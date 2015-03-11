#ifndef DEBUGSHAPEDX10_H
#define DEBUGSHAPEDX10_H

#include "configGraphicsDLL.h"
#include "color.h"
#include "vector3.h"
#include "debugShapeTypes.h"

#include <d3d10.h>

namespace Phy
{
    class BoundingVolume;
}

namespace GFX
{
    class Camera;
    struct Vertex;
    class Shader;

    class ENGINE_GRAPHICS_EXPORT DebugShape
    {
    public:
        DebugShape(unsigned int type);
        DebugShape(const DebugShape& val);
        virtual ~DebugShape();
        void Load();
        void Render(Camera* camera,
                    Math::Vec3F* offsets,
                    unsigned int offsetCount);
        void Unload();
        void RenderSetup();
    public:
        bool Enable() const;
        void Enable(bool val);
        Color ShapeColor() const;
        void ShapeColor(const Color& val);
        unsigned int Type() const;
        Phy::BoundingVolume* Shape() const;
        void Shape(Phy::BoundingVolume* val);
    protected:
        virtual void load() = 0;
        virtual void update() = 0;
        virtual void unload() = 0;
    protected:
        Phy::BoundingVolume* shape_;
        Shader* shapeShader_;
        Vertex* mappedVertexList_;
        ID3D10Buffer* vertBuffer_;
        ID3D10Buffer* indexBuffer_;
        bool enable_;
        unsigned int vertexCount_;
        unsigned int indexCount_;
        Color shapeColor_;
        unsigned int type_;
    private:
        static const unsigned int BoundingVolumeTypeMap[DS_TOTAL];
    };
}

#endif