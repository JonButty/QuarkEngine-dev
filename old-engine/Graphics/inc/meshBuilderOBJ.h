#ifndef MESHBUILDEROBJ_H
#define MESHBUILDEROBJ_H

#include "meshBuilder.h"
#include "singleton.h"
#include "delegate.h"
#include "vector3.h"
#include "configGraphicsDLL.h"
#include "face.h"
#include "halfEdge.h"
#include "vertex.h"
#include "fileInput.h"

#include <list>
#include <map>

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT OBJFormatPred 
        :   public Util::LinePredicate
    {
    public:
        OBJFormatPred(char c);
        bool operator()(const std::string& line) const;
    private:
        char data_;
    };

    class ENGINE_GRAPHICS_EXPORT MeshBuilderOBJ
        :   public MeshBuilder,
            public Util::Singleton<MeshBuilderOBJ>
    {
    public:
        MeshBuilderOBJ();
        void Parse(const std::string& file,
                   Mesh*& mesh,
                   unsigned int meshType);
    private:
        void parsePosition(Math::Vec3F& vec,
                         const std::string& line);
        void parseFace(unsigned int& index1,
                       unsigned int& index2,
                       unsigned int& index3,
                       const std::string& line);
        void processVertex(Util::DelegateParameter& param);
        void processFaceEdge(Util::DelegateParameter& param);
        void vertNeighborDelegate(Util::DelegateParameter& param);
        void countVerticesDelegate(Util::DelegateParameter& param);
        void countFacesDelegate(Util::DelegateParameter& param);
        void constructValenceList();
        void generateTriStrip();
        void processForBoundaryList(HalfEdge* edge);
        bool isFaceInOpenList( const HalfEdge* edge );
        HalfEdge* getNextUnvisitedFace(const Vertex* v);
    private:
        typedef std::map<HalfEdge*,Face*> FaceOpenList;
        typedef std::list<unsigned int> TriIndexStrip;
        typedef std::list<TriIndexStrip> TriStripList;
        typedef std::pair<unsigned int, unsigned int> EdgeInfo;
        typedef std::list<EdgeInfo> BoundaryList;
        typedef std::map<unsigned int,HalfEdge*> EdgeVertexList;
        typedef std::map<unsigned int, EdgeVertexList* >EdgeList;
    private:
        Vertex* vertexArray_;
        EdgeList* edgeList_;
        BoundaryList* boundaryIndexList_;
        Face* faceArray_;
        unsigned int vertexCount_;
        unsigned int currentVertex_;
        unsigned int faceCount_;
        unsigned int currentFace_;
        unsigned int vertNeighborCount_;
        FaceOpenList faceOpenList_;
        std::multimap<unsigned int,Vertex*> valenceList_;
        TriStripList triStripList_;
        std::list<Math::Vec3F*> currVertAdjNormalsList_;
    };
}

#endif