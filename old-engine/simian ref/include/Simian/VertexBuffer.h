/************************************************************************/
/*!
\file		VertexBuffer.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VERTEXBUFFER_H_
#define SIMIAN_VERTEXBUFFER_H_

#include "SimianPlatform.h"
#include "VertexFormat.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

#include <vector>

namespace Simian
{
    class GraphicsDevice;

    struct SIMIAN_EXPORT PositionColorVertex
    {
        f32 X, Y, Z;
        u32 Diffuse;

        static const VertexFormat Format;

        PositionColorVertex(
            const Vector3& position = Vector3(), 
            const Color& color      = Color())
            : X(position.X()),
              Y(position.Y()),
              Z(position.Z()),
              Diffuse(color.ColorU32()) {}
    };

    struct SIMIAN_EXPORT PositionColorUVVertex
    {
        f32 X, Y, Z;
        u32 Diffuse;
        f32 U, V;

        static const VertexFormat Format;

        PositionColorUVVertex(
            const Vector3& position = Vector3(), 
            const Color& color      = Color(), 
            const Vector2& texCoord = Vector2())
            : X(position.X()),
              Y(position.Y()),
              Z(position.Z()),
              Diffuse(color.ColorU32()),
              U(texCoord.X()),
              V(texCoord.Y()) {}
    };

    struct SIMIAN_EXPORT PositionNormalColorUVVertex
    {
        f32 X, Y, Z;
        f32 NX, NY, NZ;
        u32 Diffuse;
        f32 U, V;

        static const VertexFormat Format;

        PositionNormalColorUVVertex(
            const Vector3& position = Vector3(), 
            const Vector3& normal   = Vector3(), 
            const Color& color      = Color(), 
            const Vector2& texCoord = Vector2())
            : X(position.X()),
              Y(position.Y()),
              Z(position.Z()),
              NX(normal.X()),
              NY(normal.Y()),
              NZ(normal.Z()),
              Diffuse(color.ColorU32()),
              U(texCoord.X()),
              V(texCoord.Y()) {}
    };

    class SIMIAN_EXPORT VertexBufferBase
    {
    private:
        GraphicsDevice* device_;
        VertexFormat format_;
        u32 size_;
        void* data_;
        bool dynamic_;
    public:
        GraphicsDevice& Device() const { return *device_; }

        template <class T>
        T* Data() const 
        {
            return reinterpret_cast<T*>(data_);
        }

        template <class T>
        void Data(T* val, u32 size = 0, u32 offset = 0) 
        { 
            memcpy(reinterpret_cast<u8*>(data_) + offset, val, size ? size : format_.Size() * size_);
            update_(offset, size);
        }

        const Simian::VertexFormat& Format() const { return format_; }

        Simian::u32 Size() const { return size_; }

        bool Dynamic() const { return dynamic_; }
    protected:
        S_NIMPL virtual void update_(u32 offset, u32 size) = 0;
        S_NIMPL virtual bool load_() = 0;
        S_NIMPL virtual void unload_() = 0;
    public:
        VertexBufferBase(GraphicsDevice* device):
          device_(device),
          size_(0), 
          data_(0),
          dynamic_(false) {}
        virtual ~VertexBufferBase() {}

        bool Load(const VertexFormat& format, u32 size, bool dynamic = false)
        {
            format_ = format;
            size_ = size;
            data_ = new u8[format.Size() * size];
            dynamic_ = dynamic;
            if (!load_())
            {
                delete [] data_;
                data_ = 0;
                return false;
            }
            return true;
        }

        void Unload() 
        {
            unload_();

            delete [] data_;
            data_ = 0;
        }

        S_NIMPL void Set(u32 offset) { offset; }
        S_NIMPL void Unset() {}
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "VertexBufferDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "VertexBufferGL.h"
#endif

#endif
