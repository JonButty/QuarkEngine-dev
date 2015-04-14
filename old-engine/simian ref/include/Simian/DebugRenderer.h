/************************************************************************/
/*!
\file		DebugRenderer.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_DEBUGRENDERER_H_
#define SIMIAN_DEBUGRENDERER_H_

#include "SimianPlatform.h"
#if SIMIAN_DRAWDEBUGSHAPES == 1

#include "Singleton.h"
#include "Vector3.h"
#include "Delegate.h"
#include "Angle.h"
#include "Color.h"
#include "VertexBuffer.h"

#include <vector>

namespace Simian
{
    class GraphicsDevice;
    class RenderQueue;
    class Material;
    class MaterialCache;
    class Camera;
    class RenderTexture;
    class BitmapFont;
    class BitmapText;

    struct SIMIAN_EXPORT DebugVertex
    {
        f32 X, Y, Z;
        u32 Diffuse;

        DebugVertex(const Vector3& pos = Vector3::Zero, const Color& color = Color())
            : X(pos.X()), Y(pos.Y()), Z(pos.Z()), Diffuse(color.ColorU32()) {}

        static const VertexFormat Format;
    };

    class SIMIAN_EXPORT DebugRenderer: public Simian::Singleton<DebugRenderer>
    {
    private:
        // types of debug shapes
        struct DebugLine
        {
            Vector3 Start, End;
            Simian::Color Color;
        };

        struct DebugCircle
        {
            Vector3 Center;
            f32 Radius;
            Simian::Color Color;
        };

        struct DebugCube
        {
            Vector3 Center;
            Vector3 Size;
            Simian::Color Color;
        };

        struct DebugText
        {
            std::string Text;
            Vector2 Position;
            Simian::Color Color;
        };
    private:
        VertexBuffer* vertexBuffer_;
        bool showing_;

        std::vector<DebugLine> lines_;
        std::vector<DebugCircle> circles_;
        std::vector<DebugCube> cubes_;
        std::vector<DebugText> texts_;

        GraphicsDevice* device_;
        RenderQueue* renderQueue_;
        MaterialCache* materialCache_;
        Material* debugMaterial_;

        Camera* debugLayerCamera_;
        RenderTexture* debugLayer_;

        Material* debugTextMaterial_;
        BitmapFont* debugFont_;
        BitmapText* debugText_;
    public:
        void Device(GraphicsDevice* val);
        void RenderQueue(RenderQueue* val);
        void MaterialCache(MaterialCache* val);
    private:
        static void setCircleInc_(u32 i, Vector3& inc, f32 x, f32 y);

        void drawLine_(DelegateParameter& param);
        void drawCircle_(DelegateParameter& param);
        void drawCube_(DelegateParameter& param);
        void drawText_(DelegateParameter& param);
    public:
        DebugRenderer();

        bool Load();
        void Unload();

        void Show();
        void Hide();
        void ToggleVisibility();

        void ClearShapes();

        void DrawLine(const Vector3& start, const Vector3& end, const Color& col = Color());
        void DrawLineImmediate(const DebugLine& line);

        void DrawCircle(const Vector3& center, f32 radius, const Color& col = Color());
        void DrawCircleImmediate(const DebugCircle& circle);

        void DrawCube(const Vector3& center, const Vector3& size, const Color& col = Color(), const Angle& angle = Degree(0), const Vector3& axis = Vector3::UnitY);
        void DrawCubeImmediate(const DebugCube& cube);

        void DrawBitmapText(const std::string& text, const Vector2& position, const Color& col = Color());
        void DrawBitmapTextImmediate(const DebugText& text);
    };
}

#define DebugRendererLoad() Simian::DebugRenderer::Instance().Load()
#define DebugRendererUnload() Simian::DebugRenderer::Instance().Unload()
#define DebugRendererClear() Simian::DebugRenderer::Instance().ClearShapes()
#define DebugRendererShow() Simian::DebugRenderer::Instance().Show()
#define DebugRendererHide() Simian::DebugRenderer::Instance().Hide()
#define DebugRendererToggleVisibility() Simian::DebugRenderer::Instance().ToggleVisibility()
#define DebugRendererDrawLine(start, end, col) Simian::DebugRenderer::Instance().DrawLine(start, end, col)
#define DebugRendererDrawCircle(center, radius, col) Simian::DebugRenderer::Instance().DrawCircle(center, radius, col)
#define DebugRendererDrawCube(center, size, col) Simian::DebugRenderer::Instance().DrawCube(center, size, col)
#define DebugRendererDrawCubeEx(center, size, col, angle, axis) Simian::DebugRenderer::Instance().DrawCube(center, size, col, angle, axis)
#define DebugRendererDrawText(text, position, col) Simian::DebugRenderer::Instance().DrawBitmapText(text, position, col);

#else

#define DebugRendererLoad()
#define DebugRendererUnload()
#define DebugRendererClear()
#define DebugRendererShow()
#define DebugRendererHide()
#define DebugRendererToggleVisibility()
#define DebugRendererDrawLine(start, end, col)
#define DebugRendererDrawCircle(center, radius, col)
#define DebugRendererDrawCube(center, size, col)
#define DebugRendererDrawCubeEx(center, size, col, angle, axis)
#define DebugRendererDrawText(text, position, col)

#endif

#endif
