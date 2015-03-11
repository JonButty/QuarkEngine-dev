#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include "configGraphicsDLL.h"

#include <string>

namespace GFX
{
    class Mesh;

    class ENGINE_GRAPHICS_EXPORT MeshBuilder
    {
    public:
        MeshBuilder();
        virtual void Parse(const std::string& file,
                           Mesh*& mesh,
                           unsigned int meshType) = 0;
    };
}

#endif