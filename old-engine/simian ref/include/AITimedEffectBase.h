/******************************************************************************/
/*!
\file		AITimedEffectBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AITIMEDEFFECTBASE_H_
#define DESCENSION_AITIMEDEFFECTBASE_H_

#include "Simian/SimianTypes.h"
#include "AIEffectBase.h"

namespace Simian
{
    class Entity;
}

namespace Descension
{
    class AITimedEffectBase : public AIEffectBase
    {
    private:
        Simian::f32 timer_;
    public:
        Simian::f32 Timer() const;
        void Timer(Simian::f32 time);
    protected:
        virtual void onAwake_() = 0;
        virtual void onInit_( GCAttributes* source)=0;
        virtual void onUnload_()=0;
        virtual void serialize_( Simian::FileObjectSubNode& node ) = 0;
        virtual void deserialize_( const Simian::FileObjectSubNode& node ) = 0;
        virtual bool update_( Simian::f32 dt ) = 0;
    public:
        AITimedEffectBase(AIEffectType type);
        ~AITimedEffectBase();
        bool Update( Simian::f32 dt );
        virtual void Serialize( Simian::FileObjectSubNode& node );
        virtual void Deserialize( const Simian::FileObjectSubNode& node );
    };
}

#endif
