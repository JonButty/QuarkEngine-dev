/************************************************************************/
/*!
\file		IndexBuffer.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_INDEXBUFFER_H_
#define SIMIAN_INDEXBUFFER_H_

#include "SimianPlatform.h"

namespace Simian
{
    class GraphicsDevice;

    class SIMIAN_EXPORT IndexBufferBase
    {
    public:
        enum IndexType
        {
            IBT_U16 = sizeof(u16),
            IBT_U32 = sizeof(u32)
        };
    private:
        GraphicsDevice* device_;
        IndexType type_;
        void* data_;
        u32 size_;
        bool dynamic_;
    public:
        GraphicsDevice& Device() const { return *device_; }

        IndexType Type() const { return type_; }

        template <class T>
        T* Data() const { return reinterpret_cast<T*>(data_); }
        template <class T>
        void Data(T* val, u32 size = 0, u32 offset = 0) 
        { 
            memcpy(reinterpret_cast<u8*>(data_) + offset, val, size ? size : type_ * Size());
            update_(offset, size);
        }

        Simian::u32 Size() const { return size_; }

        bool Dynamic() const { return dynamic_; }
    protected:
        S_NIMPL virtual void update_(u32 offset, u32 size) = 0;
        S_NIMPL virtual bool load_() = 0;
        S_NIMPL virtual void unload_() = 0;
    public:
        IndexBufferBase(GraphicsDevice* device)
            : device_(device),
              type_(IBT_U16),
              data_(0),
              size_(0),
              dynamic_(false) {}

        virtual ~IndexBufferBase() {}

        bool Load(IndexType type, u32 size, bool dynamic = false)
        {
            type_ = type;
            size_ = size;
            data_ = new u8[type * size];
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

        S_NIMPL void Set() {}
        S_NIMPL void Unset() {}
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "IndexBufferDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "IndexBufferGL.h"
#endif

#endif
