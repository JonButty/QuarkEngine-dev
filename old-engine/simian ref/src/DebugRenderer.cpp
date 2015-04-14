/************************************************************************/
/*!
\file		DebugRenderer.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_DRAWDEBUGSHAPES == 1

#include "Simian/DebugRenderer.h"
#include "Simian/RenderQueue.h"
#include "Simian/MaterialCache.h"
#include "Simian/Material.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/LogManager.h"
#include "Simian/Math.h"
#include "Simian/Camera.h"
#include "Simian/RenderTexture.h"
#include "Simian/BitmapFont.h"
#include "Simian/BitmapText.h"

namespace Simian
{
    static const u32 BUFFER_SIZE = 1024;
    static const u32 CIRCLE_EDGES = 32;

    static VertexFormat debugVertexFormat_()
    {
        VertexFormat format;
        format.AddAttribute(VertexAttribute::VFU_POSITION, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_COLOR, VertexAttribute::VFT_COLOR);
        return format;
    }
    const VertexFormat DebugVertex::Format(debugVertexFormat_());

    //--------------------------------------------------------------------------

    DebugRenderer::DebugRenderer()
        : vertexBuffer_(0),
          showing_(false),
          device_(0),
          renderQueue_(0),
          materialCache_(0),
          debugMaterial_(0),
          debugLayerCamera_(0),
          debugLayer_(0)
    {
    }

    //--------------------------------------------------------------------------

    void DebugRenderer::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    void DebugRenderer::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    void DebugRenderer::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    //--------------------------------------------------------------------------

    void DebugRenderer::drawLine_( DelegateParameter& param )
    {
        RenderObject* ro = param.As<RenderObjectDrawParameter>().Object;
        DrawLineImmediate(lines_[reinterpret_cast<u32>(ro->UserData)]);
    }

    void DebugRenderer::drawCircle_( DelegateParameter& param )
    {
        RenderObject* ro = param.As<RenderObjectDrawParameter>().Object;
        DrawCircleImmediate(circles_[reinterpret_cast<u32>(ro->UserData)]);
    }

    void DebugRenderer::drawCube_( DelegateParameter& param )
    {
        RenderObject* ro = param.As<RenderObjectDrawParameter>().Object;
        DrawCubeImmediate(cubes_[reinterpret_cast<u32>(ro->UserData)]);
    }

    void DebugRenderer::drawText_( DelegateParameter& param )
    {
        RenderObject* ro = param.As<RenderObjectDrawParameter>().Object;
        DrawBitmapTextImmediate(texts_[reinterpret_cast<u32>(ro->UserData)]);
    }

    //--------------------------------------------------------------------------

    bool DebugRenderer::Load()
    {
        debugMaterial_ = materialCache_->Load("Materials/DebugMaterial.mat");

        if (!device_->CreateVertexBuffer(vertexBuffer_))
        {
            SWarn("DebugRenderer: Vertex buffer creation failed.");
            return false;
        }
        vertexBuffer_->Load(DebugVertex::Format, BUFFER_SIZE, true);

        // set up debug layer for text
        device_->CreateRenderTexture(debugLayer_);
        debugLayer_->LoadImmediate(device_->Size(), Texture::SF_RGBA);

        // set up camera for debug layer.
        debugLayerCamera_ = new Camera(device_);
        debugLayerCamera_->AddLayer(RenderQueue::RL_POSTUI);
        debugLayerCamera_->ClearColor(Simian::Color(1, 1, 1, 0));
        debugLayerCamera_->RenderTexture(debugLayer_);
        debugLayerCamera_->View(Matrix::Ortho(device_->Size().X(), device_->Size().Y(), -1, 1));
        debugLayerCamera_->Projection(Matrix::Identity);

        // add camera to render queue.
        renderQueue_->AddCamera(debugLayerCamera_);

        debugTextMaterial_ = materialCache_->Load("Materials/inconsolata.mat");
        debugFont_ = new BitmapFont(device_);
        debugFont_->LoadImmediate("Fonts/inconsolata.fnt");

        // create the bitmap text
        debugFont_->CreateBitmapText(debugText_);
        debugText_->Alignment(Vector2(0.0f, -1.0f));

        return true;
    }

    void DebugRenderer::Unload()
    {
        ClearShapes();

        // remove camera from render queue
        renderQueue_->RemoveCamera(debugLayerCamera_);

        debugFont_->DestroyBitmapText(debugText_);
        debugText_ = 0;

        debugFont_->Unload();
        delete debugFont_;
        debugFont_ = 0;

        // destroy camera
        delete debugLayerCamera_;
        debugLayerCamera_ = 0;

        // destroy debug layer
        debugLayer_->Unload();
        device_->DestroyRenderTexture(debugLayer_);
        debugLayer_ = 0;

        // unload vertex buffers
        vertexBuffer_->Unload();
        device_->DestroyVertexBuffer(vertexBuffer_);
    }

    void DebugRenderer::Show()
    {
        showing_ = true;
    }

    void DebugRenderer::Hide()
    {
        showing_ = false;
    }

    void DebugRenderer::ToggleVisibility()
    {
        showing_ = !showing_;
    }

    void DebugRenderer::ClearShapes()
    {
        lines_.clear();
        circles_.clear();
        cubes_.clear();
        texts_.clear();
    }

    void DebugRenderer::DrawLine( const Vector3& start, const Vector3& end, const Color& col )
    {
        DebugLine line;
        line.Start = start;
        line.End = end;
        line.Color = col;
        lines_.push_back(line);
        
        renderQueue_->AddRenderObject(
            RenderObject(*debugMaterial_, Simian::RenderQueue::RL_SCENE, Matrix::Identity,
            Delegate::CreateDelegate<DebugRenderer, &DebugRenderer::drawLine_>(this), 
            reinterpret_cast<void*>(lines_.size() - 1)));
    }

    void DebugRenderer::DrawLineImmediate( const DebugLine& line )
    {
        if (!showing_)
            return;

        DebugVertex verts[] = {
            DebugVertex(line.Start, line.Color),
            DebugVertex(line.End, line.Color)
        };
        vertexBuffer_->Data(&verts, 2 * DebugVertex::Format.Size());
        device_->Draw(vertexBuffer_, GraphicsDevice::PT_LINE, 1);
    }

    void DebugRenderer::DrawCircle( const Vector3& center, f32 radius, const Color& col )
    {
        DebugCircle circle;
        circle.Center = center;
        circle.Radius = radius;
        circle.Color = col;
        circles_.push_back(circle);

        renderQueue_->AddRenderObject(
            RenderObject(*debugMaterial_, Simian::RenderQueue::RL_SCENE, Matrix::Identity,
            Delegate::CreateDelegate<DebugRenderer, &DebugRenderer::drawCircle_>(this), 
            reinterpret_cast<void*>(circles_.size() - 1)));
    }

    void DebugRenderer::setCircleInc_(u32 i, Vector3& inc, f32 x, f32 y)
    {
        if (i == 0)
            inc = Vector3(x, y, 0);
        else if (i == 1)
            inc = Vector3(x, 0, y);
        else if (i == 2)
            inc = Vector3(0, y, x);
    }

    void DebugRenderer::DrawCircleImmediate( const DebugCircle& circle )
    {
        if (!showing_)
            return;

        f32 incAngle = (2.0f * Math::PI)/CIRCLE_EDGES;
        f32 cosInc = std::cos(incAngle);
        f32 sinInc = std::sin(incAngle);

        DebugVertex verts[CIRCLE_EDGES * 2 * 3];
        for (u32 j = 0; j < 3; ++j)
        {
            f32 startVertX = circle.Radius;
            f32 startVertY = 0;

            for (u32 i = 0; i < CIRCLE_EDGES; ++i)
            {
                Simian::Vector3 incVec;

                setCircleInc_(j, incVec, startVertX, startVertY);
                verts[j * CIRCLE_EDGES * 2 + i * 2 + 0] = DebugVertex(circle.Center + incVec, circle.Color);

                // incrementally step the vertex
                f32 tmp = startVertX * sinInc + startVertY * cosInc;
                startVertX = startVertX * cosInc + startVertY * -sinInc;
                startVertY = tmp;

                setCircleInc_(j, incVec, startVertX, startVertY);
                verts[j * CIRCLE_EDGES * 2 + i * 2 + 1] = DebugVertex(circle.Center + incVec, circle.Color);
            }
        }

        vertexBuffer_->Data(verts, DebugVertex::Format.Size() * CIRCLE_EDGES * 2 * 3);
        device_->Draw(vertexBuffer_, GraphicsDevice::PT_LINE, CIRCLE_EDGES * 3);
    }

    void DebugRenderer::DrawCube( const Vector3& center, const Vector3& size, const Color& col, const Angle& angle, const Vector3& axis )
    {
        DebugCube cube;
        cube.Center = center;
        cube.Size = size;
        cube.Color = col;
        cubes_.push_back(cube);

        Matrix transform = Matrix::Translation(center) * 
            Matrix::RotationAxisAngle(axis, angle);

        renderQueue_->AddRenderObject(
            RenderObject(*debugMaterial_, Simian::RenderQueue::RL_SCENE, transform,
            Delegate::CreateDelegate<DebugRenderer, &DebugRenderer::drawCube_>(this), 
            reinterpret_cast<void*>(cubes_.size() - 1)));
    }

    void DebugRenderer::DrawCubeImmediate( const DebugCube& cube )
    {
        if (!showing_)
            return;

        float hwidth = cube.Size.X() * 0.5f;
        float hheight = cube.Size.Y() * 0.5f;
        float hdepth = cube.Size.Z() * 0.5f;

        DebugVertex verts[] = {
            // front
            DebugVertex(Vector3(-hwidth, -hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth,  hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth,  hheight, -hdepth), cube.Color),
            DebugVertex(Vector3( hwidth,  hheight, -hdepth), cube.Color),
            DebugVertex(Vector3( hwidth,  hheight, -hdepth), cube.Color),
            DebugVertex(Vector3( hwidth, -hheight, -hdepth), cube.Color),
            DebugVertex(Vector3( hwidth, -hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth, -hheight, -hdepth), cube.Color),

            // back
            DebugVertex(Vector3(-hwidth, -hheight, hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth,  hheight, hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth,  hheight, hdepth), cube.Color),
            DebugVertex(Vector3( hwidth,  hheight, hdepth), cube.Color),
            DebugVertex(Vector3( hwidth,  hheight, hdepth), cube.Color),
            DebugVertex(Vector3( hwidth, -hheight, hdepth), cube.Color),
            DebugVertex(Vector3( hwidth, -hheight, hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth, -hheight, hdepth), cube.Color),

            // left
            DebugVertex(Vector3(-hwidth,  hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth,  hheight,  hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth, -hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(-hwidth, -hheight,  hdepth), cube.Color),

            // right
            DebugVertex(Vector3(hwidth,  hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(hwidth,  hheight,  hdepth), cube.Color),
            DebugVertex(Vector3(hwidth, -hheight, -hdepth), cube.Color),
            DebugVertex(Vector3(hwidth, -hheight,  hdepth), cube.Color),
        };

        vertexBuffer_->Data(&verts, 24 * DebugVertex::Format.Size());
        device_->Draw(vertexBuffer_, GraphicsDevice::PT_LINE, 12);
    }

    void DebugRenderer::DrawBitmapText( const std::string& text, const Vector2& position, const Color& col )
    {
        DebugText debugText;
        debugText.Text = text;
        debugText.Position = position;
        debugText.Color = col;
        texts_.push_back(debugText);

        renderQueue_->AddRenderObject(
            RenderObject(*debugTextMaterial_, RenderQueue::RL_POSTUI, Matrix::Translation(position), 
            Delegate::CreateDelegate<DebugRenderer, &DebugRenderer::drawText_>(this), 
            reinterpret_cast<void*>(texts_.size() - 1)));
    }

    void DebugRenderer::DrawBitmapTextImmediate( const DebugText& text )
    {
        debugText_->Text(text.Text);
        debugFont_->Draw(debugText_);
    }
}

#endif
