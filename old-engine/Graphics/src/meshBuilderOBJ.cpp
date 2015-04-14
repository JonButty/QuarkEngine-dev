#include "meshBuilderOBJ.h"
#include "logManager.h"
#include "mesh.h"
#include "gfxLogger.h"
#include "vertex.h"
#include "halfEdge.h"
#include "face.h"
#include "mathDef.h"

#include <algorithm>

#define VERTEX_INDEX(v) std::abs(v - &vertexArray_[0])

namespace GFX
{
    OBJFormatPred::OBJFormatPred( char c )
        : data_(c)
    {
        if(c >= min('A','Z') && c <= max('A','Z'))
            data_ = c + 'a' - 'A';
    }

    bool OBJFormatPred::operator()( const std::string& line ) const
    {
        if(line.size() == 0)
            return false;

        unsigned int spaceIndex = line.find_first_of(' ');

        if(spaceIndex > 1)
            return false;

        char lineData = line[0];
        if(lineData >= min('A','Z') && lineData <= max('A','Z'))
            lineData += 'a' - 'A';

        if(lineData == data_)
            return true;
        return false;
    }

    //--------------------------------------------------------------------------

    MeshBuilderOBJ::MeshBuilderOBJ()
        :   vertexArray_(0),
            edgeList_(0),
            faceArray_(0),
            vertexCount_(0),
            currentVertex_(0),
            faceCount_(0),
            currentFace_(0),
            vertNeighborCount_(0)
    {
    }

    void MeshBuilderOBJ::Parse( const std::string& file,
                                Mesh*& mesh,
                                unsigned int meshType)
    {
        Util::FileHandle handle;
        vertNeighborCount_ = 0;
        vertexCount_ = 0;
        currentVertex_ = 0;
        faceCount_ = 0;
        currentFace_ = 0;

        Util::FileInput::Open(handle,file);

        if(!handle.iStream_.is_open())
            return;

        GFX_LOG("Counting vertices");
        
        OBJFormatPred vertPred('V');
        Util::FileInput::Parse(handle,
                               &vertPred,
                               Util::Delegate::CreateDelegate<MeshBuilderOBJ,&MeshBuilderOBJ::countVerticesDelegate>(this));
        
        vertexArray_ = new Vertex[vertexCount_];
        edgeList_ = new EdgeList;
        boundaryIndexList_ = new BoundaryList;

        GFX_LOG("Parsing vertices");
        Util::FileInput::Parse(handle,
                               &vertPred,
                               Util::Delegate::CreateDelegate<MeshBuilderOBJ,&MeshBuilderOBJ::processVertex>(this));

        GFX_LOG("Counting faces");
        OBJFormatPred facePred('f');
        Util::FileInput::Parse(handle,
                               &facePred,
                               Util::Delegate::CreateDelegate<MeshBuilderOBJ,&MeshBuilderOBJ::countFacesDelegate>(this));
        
        faceArray_ = new Face[faceCount_];

        GFX_LOG("Parsing faces and edges");
        Util::FileInput::Parse(handle,
                               &facePred,
                               Util::Delegate::CreateDelegate<MeshBuilderOBJ,&MeshBuilderOBJ::processFaceEdge>(this));
        
        Util::FileInput::Close(handle);


        GFX_LOG("Constructing valence list");
        constructValenceList();

        GFX_LOG("Generating tri strip");
        generateTriStrip();

        valenceList_.clear();

        GFX_LOG("Copying tri strip");
        
        // Tri strip
        unsigned int stripCount = triStripList_.size();
        unsigned int** indexArray = new unsigned int* [stripCount];
        std::list<unsigned int>* stripSizes = new std::list<unsigned int>;

        std::list<TriIndexStrip>::iterator stripIt = triStripList_.begin();
        for(unsigned int i = 0; stripIt != triStripList_.end(); ++stripIt, ++i)
        {
            stripSizes->push_back(stripIt->size());
            indexArray[i] = new unsigned int [stripIt->size()];

            Color stripColor(std::abs(Math::RandomFloat()),std::abs(Math::RandomFloat()),std::abs(Math::RandomFloat()),1);
            
            TriIndexStrip::iterator indexIt = stripIt->begin();
            for(unsigned int j = 0; indexIt != stripIt->end(); ++indexIt, ++j)
            {
                indexArray[i][j] = *indexIt;
                vertexArray_[*indexIt].color_ = stripColor;
            }
        }
        
        // Boundary edge 
        unsigned int boundaryIndexCount = boundaryIndexList_->size() * 2;
        unsigned int* boundaryIndexArray = new unsigned int[boundaryIndexCount];

        BoundaryList::iterator edgeIt = boundaryIndexList_->begin();
        for(unsigned int i = 0; edgeIt != boundaryIndexList_->end(); ++edgeIt, ++i)
        {
            boundaryIndexArray[i] = edgeIt->first;
            boundaryIndexArray[++i] = edgeIt->second;
        }
        mesh = new Mesh(meshType,
                        vertexArray_,
                        vertexCount_,
                        indexArray,
                        stripSizes,
                        boundaryIndexArray,
                        boundaryIndexCount);

        delete faceArray_;
        faceArray_ = 0;

        unsigned int listSize = edgeList_->size();
        for(unsigned int i = 0; i < listSize; ++i)
        {
            std::map<unsigned int,HalfEdge*>** list2 = &(*edgeList_)[i];
            if(!*list2)
                continue;
            
            std::map<unsigned int,HalfEdge*>::iterator it = (*list2)->begin();
            for(; it != (*list2)->end(); ++it)
            {
                delete it->second;
                it->second = 0;
            }
            delete *list2;
            *list2 = 0;
        }
        delete edgeList_;
        edgeList_ = 0;

        delete boundaryIndexList_;
        boundaryIndexList_ = 0;

        triStripList_.clear();
    }

    void MeshBuilderOBJ::parsePosition( Math::Vec3F& vec,
                                        const std::string& line)
    {
        unsigned int leftBound, rightBound;

        leftBound = line.find_first_not_of(' ',1) ;
        rightBound = line.find_first_of(' ', leftBound + 1);
        vec.X(static_cast<float>(std::atof(line.substr(leftBound, rightBound - leftBound).c_str())));

        leftBound = line.find_first_not_of(' ', rightBound);
        rightBound = line.find_first_of(' ', leftBound + 1);
        vec.Y(static_cast<float>(std::atof(line.substr(leftBound, rightBound - leftBound).c_str())));

        leftBound = line.find_first_not_of(' ', rightBound);
        rightBound = line.find_first_of(' ', leftBound + 1);
        vec.Z(static_cast<float>(std::atof(line.substr(leftBound, rightBound - leftBound).c_str())));
    }

    void MeshBuilderOBJ::parseFace( unsigned int& index1,
                                    unsigned int& index2, 
                                    unsigned int& index3,
                                    const std::string& line )
    {
        unsigned int leftBound, rightBoundMin, spaceIndex, slashIndex;

        leftBound = line.find_first_not_of(' ',1);
        spaceIndex = line.find_first_of(' ', leftBound + 1);
        slashIndex = line.find_first_of('/', leftBound + 1);
        rightBoundMin = min(spaceIndex,slashIndex);
        index1 = static_cast<unsigned int>(std::atoi(line.substr(leftBound,rightBoundMin - leftBound).c_str())) - 1;

        leftBound = line.find_first_not_of(' ',spaceIndex);
        spaceIndex = line.find_first_of(' ', leftBound + 1);
        slashIndex = line.find_first_of('/', leftBound + 1);
        rightBoundMin = min(spaceIndex,slashIndex);
        index2 = static_cast<unsigned int>(std::atoi(line.substr(leftBound,rightBoundMin - leftBound).c_str())) - 1;

        leftBound = line.find_first_not_of(' ',spaceIndex);
        spaceIndex = line.find_first_of(' ', leftBound + 1);
        slashIndex = line.find_first_of('/', leftBound + 1);
        index3 = static_cast<unsigned int>(std::atoi(line.substr(leftBound,rightBoundMin - leftBound).c_str())) - 1;
    }

    void MeshBuilderOBJ::processVertex( Util::DelegateParameter& param )
    {
        std::string line(param.As<Util::StringDelegateParam>().string_);
        
        Vertex& vert(vertexArray_[currentVertex_++]);
        parsePosition(vert.pos_,line);
        vert.color_ = Color(0,0,0,0);
    }

    void MeshBuilderOBJ::processFaceEdge( Util::DelegateParameter& param )
    {
        std::string line(param.As<Util::StringDelegateParam>().string_);
        unsigned int indices[3];
        parseFace(indices[0],indices[1],indices[2],line);

        Face* currFace = &faceArray_[currentFace_++];
        HalfEdge* currFaceEdges[3] = {NULL, NULL, NULL};
        
        // Error check: more than 2 faces are sharing one edge and it's twin
        for(unsigned int i = 0; i < 3; ++i)
        {
            unsigned int prev = indices[i];
            unsigned int next = indices[(i + 1) % 3];

            // Edge going to vertex at index next
            EdgeList::iterator fromVertexIt = edgeList_->find(prev);
            if(fromVertexIt == edgeList_->end())
                edgeList_->insert(std::pair<unsigned int,EdgeVertexList*>(prev,new EdgeVertexList));

            // If edge does not exist there is a chance that the twin does not
            // exist as well
            EdgeVertexList* toVertexList = edgeList_->find(prev)->second;
            EdgeVertexList::iterator toVertexIt = toVertexList->find(next);
            HalfEdge* edge = 0;

            // An edge already exists
            if(toVertexIt != toVertexList->end())
            {
                // Edge exists so use it as the curredge if it is the uninitialized
                // twin of another edge
                if(!toVertexIt->second->face_)
                    edge = toVertexIt->second;

                // index out of order
                // 2 adjacent faces given in different winding orders
                else if(!toVertexIt->second->twin_->face_)
                {
                    edge = toVertexIt->second->twin_;
                    std::swap(indices[i],indices[(i + 1) % 3]);
                    //GFX_LOG("Warning!");
                }

                else 
                {
                    //GFX_LOG("Error, more than 2 faces are sharing one edge and it's twin");
                    //GFX_LOG("\t" << line << " with edge " << prev << " ," << next);
                    return;
                }
                currFaceEdges[i] = edge;
            }
        }

        // Process remaining edges 
        for(unsigned int i = 0; i < 3; ++i)
        {
            // Edge exists and has already been initialized
            if(currFaceEdges[i])
                continue;

            unsigned int prev = indices[i];
            unsigned int next = indices[(i + 1) % 3];

            // Edge going to vertex at index next
            EdgeList::iterator fromVertexIt = edgeList_->find(prev);
            EdgeVertexList* toVertexList = fromVertexIt->second;
            EdgeVertexList::iterator toVertexIt = toVertexList->find(next);
            HalfEdge* edge = new HalfEdge;
            edge->vert_ = &vertexArray_[next];

            if(!vertexArray_[prev].edge_)
                vertexArray_[prev].edge_ = edge;

            toVertexList->insert(std::pair<unsigned int,HalfEdge*>(next,edge));
                
            // Twin edge going to vertex at index prev
            fromVertexIt = edgeList_->find(next);
            if(fromVertexIt == edgeList_->end())
                edgeList_->insert(std::pair<unsigned int,EdgeVertexList*>(next,new EdgeVertexList));

            // Check if twin edge does not exist
            toVertexList = edgeList_->find(next)->second;
            toVertexIt = toVertexList->find(prev);
            HalfEdge* twin = 0;
            if(toVertexIt == toVertexList->end())
            {
                twin = new HalfEdge;
                twin->vert_ = &vertexArray_[prev];
                twin->twin_ = edge;
                toVertexList->insert(std::pair<unsigned int,HalfEdge*>(prev,twin));
            }
            else 
                twin = toVertexIt->second;
            edge->twin_ = twin;
            currFaceEdges[i] = edge;
        }
        
        // Assign the next pointer to all edges in the current face and edge
        // pointers to current face's vertices
        for(unsigned int i = 0; i < 3; ++i)
        {
            currFaceEdges[i]->next_ = currFaceEdges[(i + 1) % 3];
            currFaceEdges[i]->face_ = currFace;
        }

        // Calculate the face's normal
        Math::Vec3F edge1(currFaceEdges[0]->vert_->pos_ - currFaceEdges[2]->vert_->pos_);
        Math::Vec3F edge2(currFaceEdges[1]->vert_->pos_ - currFaceEdges[2]->vert_->pos_);
        currFace->norm_ = edge1.Cross(edge2).Normalized();
        
        // if an existing face is already pointing to this edge, reassign
        for(unsigned int i = 0; i < 3; ++i)
        {
            if(faceOpenList_.find(currFaceEdges[i]) == faceOpenList_.end())
            {
                currFace->edge_ = currFaceEdges[i];
                faceOpenList_.insert(std::pair<HalfEdge*,Face*>(currFaceEdges[i],currFace));
                break;
            }
        }
    }

    void MeshBuilderOBJ::vertNeighborDelegate( Util::DelegateParameter& param)
    {
        HalfEdge* edge = param.As<EdgeParam>().edge_;
        if(!edge)
            return;

        ++vertNeighborCount_;
        Math::Vec3F* normal = 0;
        
        if(!edge->face_)
            normal = &edge->twin_->face_->norm_;
        else
            normal = &edge->face_->norm_;

        std::list<Math::Vec3F*>::iterator normalsIt = currVertAdjNormalsList_.begin();
        while(normalsIt != currVertAdjNormalsList_.end())
        {
            const Math::Vec3F& currNormal = *(*normalsIt);

            // Eliminate duplicate normals
            if((currNormal - *normal).SquareLength() <= Math::EPSILON)
                return;
            ++normalsIt;
        }
        currVertAdjNormalsList_.push_back(normal);
    }

    void MeshBuilderOBJ::countVerticesDelegate( Util::DelegateParameter& )
    {
        ++vertexCount_;
    }

    void MeshBuilderOBJ::countFacesDelegate( Util::DelegateParameter& )
    {
        ++faceCount_;
    }

    void MeshBuilderOBJ::constructValenceList()
    {
        //Util::Delegate vertCountCallback = Util::Delegate::CreateDelegate<MeshBuilderOBJ,&MeshBuilderOBJ::vertNeighborDelegate>(this); 
        for(unsigned int i = 0; i < vertexCount_; ++i)
        {
            vertNeighborCount_ = 0;
            currVertAdjNormalsList_.clear();
            EdgeList::iterator fromIt = edgeList_->find(i);

            if(fromIt != edgeList_->end())
            {
                EdgeVertexList::iterator toIt = fromIt->second->begin();
                while(toIt != fromIt->second->end())
                {
                    EdgeParam param;
                    param.edge_ = toIt->second;
                    vertNeighborDelegate(param);
                    ++toIt;
                }
                // Construct current vert's normal
                std::list<Math::Vec3F*>::iterator normalsIt = currVertAdjNormalsList_.begin();
                for(; normalsIt != currVertAdjNormalsList_.end(); ++normalsIt)
                    vertexArray_[i].norm_ += *(*normalsIt);
                vertexArray_[i].norm_.Normalize();

                valenceList_.insert(std::pair<unsigned int,Vertex*>(vertNeighborCount_,&vertexArray_[i]));
            } 
            
            /*
            OneRingTraversal(&vertexArray_[i],vertCountCallback);

            // Construct current vert's normal
            std::list<Math::Vec3F*>::iterator normalsIt = currVertAdjNormalsList_.begin();
            for(; normalsIt != currVertAdjNormalsList_.end(); ++normalsIt)
                vertexArray_[i].norm_ += *(*normalsIt);
            vertexArray_[i].norm_.Normalize();

            valenceList_.insert(std::pair<unsigned int,Vertex*>(vertNeighborCount_,&vertexArray_[i]));
            */
        }
    }

    void MeshBuilderOBJ::generateTriStrip()
    {
        // Loop until all faces have been put into strips
        while (!faceOpenList_.empty())
        {
            // Begin a new strip
            std::multimap<unsigned int,Vertex*>::iterator vertIt = valenceList_.begin();
            unsigned int valence = vertIt->first;
            Vertex* currVert = vertIt->second;

            valenceList_.erase(valenceList_.begin());
            
            if(valence > 2)
                valenceList_.insert(std::pair<unsigned int,Vertex*>(valence - 2,currVert));

            // Check for valid face
            HalfEdge* currEdge =  getNextUnvisitedFace(currVert);
            
            if(!currEdge)
                continue;

            HalfEdge* nextEdge = currEdge->next_;
            unsigned int faceCount = 1;

            TriIndexStrip strip;

            strip.push_back(VERTEX_INDEX(currVert));
            strip.push_back(VERTEX_INDEX(currEdge->vert_));
            strip.push_back(VERTEX_INDEX(nextEdge->vert_));

            // Boundary edge
            processForBoundaryList(currEdge->prev());
            processForBoundaryList(currEdge);
            processForBoundaryList(nextEdge);

            FaceOpenList::iterator faceIt = faceOpenList_.find(currEdge->face_->edge_);
            faceOpenList_.erase(faceIt);

            currEdge = nextEdge;
            
            // Loop through rest of the strip
            while(!faceOpenList_.empty())
            {
                ++faceCount;

                // Odd count
                if(faceCount % 2)
                    nextEdge = currEdge->next_;
                else
                    nextEdge = currEdge;

                // Boundary edge
                // End of strip
                if(nextEdge->isBoundaryEdge())
                {
                    processForBoundaryList(nextEdge);
                    break;
                }

                // Check for previously visited face
                faceIt = faceOpenList_.find(nextEdge->twin_->next_->face_->edge_);
                if(faceIt == faceOpenList_.end())
                    break;

                faceOpenList_.erase(faceIt);
                strip.push_back(VERTEX_INDEX(nextEdge->twin_->next_->vert_));
                currEdge = nextEdge->twin_->next_;
            }
            triStripList_.push_back(strip);
        }
    }

    void MeshBuilderOBJ::processForBoundaryList( HalfEdge* edge )
    {
        if(edge->isBoundaryEdge())
            boundaryIndexList_->push_back(std::pair<unsigned int, unsigned int>(VERTEX_INDEX(edge->vert_),VERTEX_INDEX(edge->prev()->vert_)));
    }

    bool MeshBuilderOBJ::isFaceInOpenList( const HalfEdge* edge ) 
    {
        if(!edge->face_)
            return false;

        FaceOpenList::iterator faceIt = faceOpenList_.find(edge->face_->edge_);
        if(faceIt == faceOpenList_.end())
            return false;
        return true;
    }

    HalfEdge* MeshBuilderOBJ::getNextUnvisitedFace( const Vertex* v )
    {
        EdgeList::iterator fromIt = edgeList_->find(VERTEX_INDEX(v));

        if(fromIt != edgeList_->end())
        {
            EdgeVertexList::iterator toIt = fromIt->second->begin();
            while(toIt != fromIt->second->end())
            {
                if(isFaceInOpenList(toIt->second))
                    return toIt->second;
                ++toIt;
            }
        } 
        return NULL;
    }
}