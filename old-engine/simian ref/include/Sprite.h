/*************************************************************************/
/*!
\file		Sprite.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SPRITE_H_
#define DESCENSION_SPRITE_H_

#include "Simian/Vector2.h"
#include "Simian/Matrix.h"
#include "Simian/SimianTypes.h"

#include <vector>
#include <algorithm>
#include <string>

namespace Descension
{
    //A simple 2D Sprite Class using Vertex Buffer
    class Sprite
    {
    public: //public static consts
        static Simian::u32 totalSpriteCount;

        enum MOUSE_HIT
        {
            MOUSE_MISS = 0,
            MOUSE_HIT,
            MOUSE_PRESS,
            MOUSE_RELEASE,

            HOVER_STATE_TOTAL
        };
        
    public: //public accessors / gettors
        void Name(const std::string& input);
        const std::string& Name(void) const;
        void SpriteID(Simian::u32 input);
        Simian::u32 SpriteID(void) const;
        void Frame(Simian::u32 input);
        Simian::u32 Frame(void) const;
        void Rotation(Simian::f32 input);
        Simian::f32 Rotation(void) const;
        void Opacity(Simian::f32 input);
        Simian::f32 Opacity(void) const;
        void Scale(const Simian::Vector2& input);
        void Scale(Simian::f32 input); //--overload float
        const Simian::Vector2& Scale(void)  const;
        void BoundingBox(const Simian::Vector2& input);
        const Simian::Vector2& BoundingBox(void)  const;
        void Position(const Simian::Vector2& input);
        const Simian::Vector2& Position(void) const;
        void Pivot(const Simian::Vector2& input); //!--offset from center
        const Simian::Vector2& Pivot(void) const;
        //--------------------------------------------
        template <class T> void UV(const T* input)
        {
            for (Simian::u32 i=0; i<4; ++i)
            {
                uv_[i] = (*input)[i];
            }
        }
        //--------------------------------------------
        const Simian::Vector2* UV(void) const;

        operator const char*() const //name finder
        {
            return name_.c_str();
        }

    private: //private data
        std::string name_;
        Simian::u32 spriteID_, frame_, mouseHit_;

        Simian::u32 statusFlag_;

        Simian::f32 rotation_, opacity_;
        Simian::Vector2 scale_;
        Simian::Vector2 boundingBox_;
        Simian::Vector2 position_, pivot_;
        Simian::Vector2 uv_[4];

        Simian::Matrix transform_;

    public: //public methods and ctors
        Sprite() : name_("Sprite"), spriteID_(0), frame_(0),
            mouseHit_(MOUSE_MISS), rotation_(0.0f), opacity_(0.0f)
        {
            //let everything default construct
            ++totalSpriteCount;
        }
        
        Sprite( const std::string& name,
            const Simian::u32 spriteID, //non-default constructor
            const Simian::u32 frame,
            const Simian::f32 rotation,
            const Simian::f32 opacity,
            const Simian::Vector2& scale,
            const Simian::Vector2& boundingBox,
            const Simian::Vector2& position,
            const Simian::Vector2& pivot) :
            name_(name), spriteID_(spriteID), frame_(frame), mouseHit_(MOUSE_MISS),
            rotation_(rotation), opacity_(opacity), scale_(scale),
            boundingBox_(boundingBox), position_(position), pivot_(pivot)
        {
            //member init
            ++totalSpriteCount;
        }

        Sprite(const Sprite& rhs) :
            name_(rhs.name_), spriteID_(rhs.spriteID_), frame_(rhs.frame_),
            mouseHit_(MOUSE_MISS), rotation_(rhs.rotation_), opacity_(rhs.opacity_),
            scale_(rhs.scale_), boundingBox_(rhs.boundingBox_), pivot_(rhs.pivot_),
            position_(rhs.position_), transform_(rhs.transform_)
        {
            //member init
            memcpy(uv_, rhs.uv_, 4*sizeof(Simian::Vector2));
            ++totalSpriteCount;
        }

        ~Sprite()
        {
            --totalSpriteCount;
        }

        //----------------------------public methods----------------------------
        const Simian::Matrix& Update(void);
        bool OnRollOver(const Simian::Vector2& mouseVec);
        bool OnHover(const Simian::Vector2& mouseVec);
        bool OnRollOut(const Simian::Vector2& mouseVec);
        bool OnPress(const Simian::Vector2& mouseVec);
        bool OnRelease(const Simian::Vector2& mouseVec);

        static void CreateSprite(std::vector<Sprite>& sprites,
            Simian::u32 id,
            const std::string& name = "Sprite",
            Simian::f32 posX = 0.0f,
            Simian::f32 posY = 0.0f,
            Simian::f32 width = 0.0f,
            Simian::f32 height = 0.0f,
            Simian::f32 scaleX = 1.0f,
            Simian::f32 scaleY = 1.0f,
            Simian::f32 rotation = 0.0f,
            Simian::f32 opacity = 1.0f,
            Simian::f32 pivotX = 0.0f,
            Simian::f32 pivotY = 0.0f);
        static void DeleteSprite(std::vector<Sprite>& sprites, Simian::u32 id);
        static Sprite& GetSprite(std::vector<Sprite>& sprites,
                                 const std::string& name);
        static Sprite* GetSpritePtr(std::vector<Sprite>& sprites,
                                    const std::string& name);
    };
}

#endif
