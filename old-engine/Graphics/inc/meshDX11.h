#ifndef MESHDX11_H
#define MESHDX11_H

#include "configGraphicsDLL.h"

#include <d3d11.h>
#include <map>
#include <list>

namespace GFX
{
    struct Vertex;
    struct Face;
    struct HalfEdge;
    class Shader;
    class Camera;

    class ENGINE_GRAPHICS_EXPORT Mesh
    {
    public:
        Mesh(Vertex*& vertList,
             unsigned int vertCount,
             unsigned int**& indexList,
             std::list<unsigned int>*& stripSizes,
             std::map<unsigned int, std::map<unsigned int,HalfEdge*>* >* edgeList,
             Face*& faceList_,
             unsigned int faceCount);
        Mesh(const Mesh& val);
        ~Mesh();
        void Load();
        void Render(Shader* shader,
                    Camera* camera);
        void Unload();
    private:
        ID3D11Buffer* vertBuffer_;
        ID3D11Buffer** indexBuffer_;
        Vertex* vertList_;
        unsigned int** indexList_;
        std::list<unsigned int>* stripSizes_;
        // Edge list is a 2D array of HalfEdge*
        // Each dimension is of size vertCount
        std::map<unsigned int, std::map<unsigned int,HalfEdge*>* >* edgeList_; 
        Face* faceList_;
        unsigned int vertCount_;
        unsigned int faceCount_;
    };
}

#endif