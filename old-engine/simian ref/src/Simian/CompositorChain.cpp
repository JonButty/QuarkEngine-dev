/************************************************************************/
/*!
\file		CompositorChain.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CompositorChain.h"
#include "Simian/Compositor.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/RenderTexture.h"
#include <algorithm>
#include "Simian/LogManager.h"

namespace Simian
{
    CompositorChain::CompositorChain( GraphicsDevice* device, VertexBuffer* fullscreenQuad )
        : device_(device),
          fullscreenQuad_(fullscreenQuad)
    {
    }

    //--------------------------------------------------------------------------

    RenderTexture* CompositorChain::Output() const
    {
        return compositors_.size() ? compositors_.back()->Output() : input_;
    }

    RenderTexture* CompositorChain::Input() const
    {
        return input_;
    }

    void CompositorChain::Input( RenderTexture* val )
    {
        input_ = val;
        if (compositors_.size())
            compositors_[0]->previous_ = input_;
    }

    const std::vector<Compositor*>& CompositorChain::Compositors() const
    {
        return compositors_;
    }

    //--------------------------------------------------------------------------

    std::vector<Compositor*>::iterator CompositorChain::unlinkCompositor_( Compositor* compositor )
    {
        std::vector<Compositor*>::iterator i = std::find(compositors_.begin(),
            compositors_.end(), compositor);
        if (i == compositors_.end())
        {
            SWarn("Compositor not found in chain.");
            return compositors_.end();
        }        
        if ((i + 1) != compositors_.end())
            (*(i + 1))->previous_ = i == compositors_.begin() ? input_ : 
                (*(i - 1))->Output();
        return i;
    }

    //--------------------------------------------------------------------------

    void CompositorChain::AddCompositor( Compositor* compositor )
    {
        compositor->previous_ = compositors_.size() ? 
            compositors_.back()->Output() : input_;
        compositors_.push_back(compositor);
        compositor->chain_ = this;
    }

    void CompositorChain::RemoveCompositor( Compositor* compositor )
    {
        std::vector<Compositor*>::iterator i = unlinkCompositor_(compositor);
        if (i != compositors_.end())
            compositors_.erase(i);
    }

    void CompositorChain::EnableCompositor( Compositor* compositor )
    {
        std::vector<Compositor*>::iterator i = std::find(compositors_.begin(),
            compositors_.end(), compositor);
        if (i == compositors_.end())
        {
            SWarn("Compositor not found in chain.");
            return;
        }
        if (i + 1 != compositors_.end())
            (*(i + 1))->previous_ = (*i)->Output();
    }

    void CompositorChain::DisableCompositor( Compositor* compositor )
    {
        unlinkCompositor_(compositor);
    }

    void CompositorChain::Render()
    {
        // render all compositors
        for (Simian::u32 i = 0; i < compositors_.size(); ++i)
            compositors_[i]->Render();
    }

    void CompositorChain::DrawFullscreenQuad()
    {
        device_->Draw(fullscreenQuad_, Simian::GraphicsDevice::PT_TRIANGLESTRIP, 2);
    }
}
