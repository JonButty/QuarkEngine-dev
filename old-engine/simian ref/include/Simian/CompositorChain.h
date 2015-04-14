/************************************************************************/
/*!
\file		CompositorChain.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_COMPOSITORCHAIN_H_
#define SIMIAN_COMPOSITORCHAIN_H_

#include "SimianPlatform.h"

#include <vector>

namespace Simian
{
    class Compositor;
    class RenderTexture;
    class VertexBuffer;
    class GraphicsDevice;

    class SIMIAN_EXPORT CompositorChain
    {
    private:
        std::vector<Compositor*> compositors_;
        VertexBuffer* fullscreenQuad_;
        GraphicsDevice* device_;
        RenderTexture* input_;
    public:
        RenderTexture* Output() const;

        RenderTexture* Input() const;
        void Input(RenderTexture* val);

        const std::vector<Compositor*>& Compositors() const;
    private:
        std::vector<Compositor*>::iterator unlinkCompositor_(Compositor* compositor);
    public:
        CompositorChain(GraphicsDevice* device, VertexBuffer* fullscreenQuad);

        void AddCompositor(Compositor* compositor);
        void RemoveCompositor(Compositor* compositor);

        void EnableCompositor(Compositor* compositor);
        void DisableCompositor(Compositor* compositor);

        // chain renders every compositor
        void Render();

        void DrawFullscreenQuad();
    };
}

#endif
