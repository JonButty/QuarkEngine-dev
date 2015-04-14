/*************************************************************************/
/*!
\file		VisualTile.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_VISUALTILE_H_
#define DESCENSION_VISUALTILE_H_

#include "Simian/Vector2.h"
#include "Simian/Matrix.h"

namespace Simian
{
    class VertexBuffer;
    class Material;
    struct DelegateParameter;
}

namespace Descension
{
    struct Tile;

    class VisualTile
    {
    private:
        static Simian::VertexBuffer* unitCube_;
        Simian::Material* material_;
        Tile* tile_;
        Simian::Vector2 position_;
    private:
        void draw_(Simian::DelegateParameter& param);
        void drawPiece_(const Simian::Matrix& transform);
    public:
        VisualTile(const Simian::Vector2& position, Tile* tile, Simian::Material* material);

        static void LoadShared();
        static void UnloadShared();

        void Draw();
    };
}

#endif
