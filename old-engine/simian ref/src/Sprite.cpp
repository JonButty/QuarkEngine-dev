/*************************************************************************/
/*!
\file		Sprite.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Sprite.h"

#include "Simian/Angle.h"
#include "Simian/Mouse.h"

namespace Descension
{
    //useful consts
    Simian::u32 Sprite::totalSpriteCount = 0;

    //--------------------------------------------------------------------------
    void Sprite::Name(const std::string& input)
    {
        name_ = input;
    }
    const std::string& Sprite::Name(void) const
    {
        return name_;
    }
    void Sprite::SpriteID(Simian::u32 input)
    {
        spriteID_ = input;
    }
    Simian::u32 Sprite::SpriteID(void) const
    {
        return spriteID_;
    }
    void Sprite::Frame(Simian::u32 input)
    {
        spriteID_ = input;
    }
    Simian::u32 Sprite::Frame(void) const
    {
        return spriteID_;
    }
    void Sprite::Rotation(Simian::f32 input)
    {
        rotation_ = input;
    }
    Simian::f32 Sprite::Rotation(void) const
    {
        return rotation_;
    }
    void Sprite::Opacity(Simian::f32 input)
    {
        opacity_ = input;
    }
    Simian::f32 Sprite::Opacity(void) const
    {
        return opacity_;
    }
    void Sprite::Scale(const Simian::Vector2& input)
    {
        scale_ = input;
    }
    void Sprite::Scale(Simian::f32 input)
    {
        scale_ = Simian::Vector2(input, input);
    }
    const Simian::Vector2& Sprite::Scale(void) const
    {
        return scale_;
    }
    void Sprite::BoundingBox(const Simian::Vector2& input)
    {
        boundingBox_ = input;
    }
    const Simian::Vector2& Sprite::BoundingBox(void) const
    {
        return boundingBox_;
    }
    void Sprite::Position(const Simian::Vector2& input)
    {
        position_ = input;
    }
    const Simian::Vector2& Sprite::Position(void) const
    {
        return position_;
    }
    void Sprite::Pivot(const Simian::Vector2& input)
    {
        pivot_ = input;
    }
    const Simian::Vector2& Sprite::Pivot(void) const
    {
        return pivot_;
    }
    const Simian::Vector2* Sprite::UV(void) const
    {
        return uv_;
    }

    const Simian::Matrix& Sprite::Update(void)
    {
        //update transform_ matrix for drawing
        Simian::Matrix concat;
        concat.Scale(scale_);
        transform_ = concat;
        concat.Rotation2D(Simian::Degree(rotation_));
        transform_ *= concat;
        concat.Translation(position_);
        transform_ *= concat;

        return transform_;
    }
    bool Sprite::OnRollOver(const Simian::Vector2& mouseVec)
    {
        if (mouseHit_ != MOUSE_HIT &&
            mouseVec.X() > position_.X()-boundingBox_.X()*0.5f - Pivot().X() &&
            mouseVec.X() < position_.X()+boundingBox_.X()*0.5f - Pivot().X() &&
            mouseVec.Y() > position_.Y()-boundingBox_.Y()*0.5f - Pivot().Y() &&
            mouseVec.Y() < position_.Y()+boundingBox_.Y()*0.5f - Pivot().Y() )
        {
            mouseHit_ = MOUSE_HIT;
            return true;
        }
        OnHover(mouseVec);
        return false;
    }
    bool Sprite::OnHover(const Simian::Vector2& mouseVec)
    {
        if (mouseVec.X() > position_.X()-boundingBox_.X()*0.5f - Pivot().X() &&
            mouseVec.X() < position_.X()+boundingBox_.X()*0.5f - Pivot().X() &&
            mouseVec.Y() > position_.Y()-boundingBox_.Y()*0.5f - Pivot().Y() &&
            mouseVec.Y() < position_.Y()+boundingBox_.Y()*0.5f - Pivot().Y() )
        {
            mouseHit_ = MOUSE_HIT;
            return true;
        }
        mouseHit_ = false;
        return false;
    }
    bool Sprite::OnRollOut(const Simian::Vector2& mouseVec)
    {
        if (mouseHit_ != MOUSE_HIT &&
            mouseVec.X() < position_.X()-boundingBox_.X()*0.5f - Pivot().X() ||
            mouseVec.X() > position_.X()+boundingBox_.X()*0.5f - Pivot().X() ||
            mouseVec.Y() < position_.Y()-boundingBox_.Y()*0.5f - Pivot().Y() ||
            mouseVec.Y() > position_.Y()+boundingBox_.Y()*0.5f - Pivot().Y() )
        {
            mouseHit_ = MOUSE_MISS;
            return true;
        }
        OnHover(mouseVec);
        return false;
    }
    bool Sprite::OnPress(const Simian::Vector2& mouseVec)
    {
        return Simian::Mouse::IsPressed(Simian::Mouse::KEY_LBUTTON) &&
            OnHover(mouseVec);
    }
    bool Sprite::OnRelease(const Simian::Vector2& mouseVec)
    {
        return Simian::Mouse::IsReleased(Simian::Mouse::KEY_LBUTTON) &&
            OnHover(mouseVec);
    }

    //--------------------------------------------------------------------------
    //--Sprites creation
    void Sprite::CreateSprite(std::vector<Sprite>& sprites,
        Simian::u32 id,
        const std::string& name,
        Simian::f32 posX,
        Simian::f32 posY,
        Simian::f32 width,
        Simian::f32 height,
        Simian::f32 scaleX,
        Simian::f32 scaleY,
        Simian::f32 rotation,
        Simian::f32 opacity,
        Simian::f32 pivotX,
        Simian::f32 pivotY)
    {
        sprites.push_back(Sprite(name, id, 0, rotation, opacity,
            Simian::Vector2(scaleX, scaleY), Simian::Vector2(width, height),
            Simian::Vector2(posX, posY), Simian::Vector2(pivotX, pivotY)));
    }
    void Sprite::DeleteSprite(std::vector<Sprite>& sprites, Simian::u32 id)
    {
        for (Simian::u32 i=id+1; i<sprites.size(); ++i)
            sprites[i].SpriteID(i-1);
        sprites.erase(sprites.begin()+id);
    }
    Sprite& Sprite::GetSprite(std::vector<Sprite>& sprites,
                            const std::string& name)
    {
        Sprite* temp = GetSpritePtr(sprites, name);
        if (!temp)
        {
            SAssert(false, "Sprites Error: Cannot find by name!");
        }
        return *temp;
    }
    Sprite* Sprite::GetSpritePtr(std::vector<Sprite>& sprites,
                                 const std::string& name)
    {
        for (Simian::u32 i=0; i<sprites.size(); ++i)
        {
            if (sprites[i].Name() == name)
                return &(sprites[i]);
        }
        return NULL;
    }
}
