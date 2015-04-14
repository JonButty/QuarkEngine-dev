/*************************************************************************/
/*!
\file		AISkillBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_AISKILLBASE_H_
#define DESCENSION_AISKILLBASE_H_

#include "Simian/SimianTypes.h"

#include "AIEffectBase.h"
namespace Descension
{
    class AISkillBase : public AIEffectBase
    {
    private:
        Simian::u32 chance_;
    public:
        Simian::u32 Chance() const;
        void Chance(Simian::u32 val);
    protected:
        virtual void onAwake_() = 0;
        virtual void onInit_( GCAttributes* source )=0;
        virtual void onUnload_()=0;
        virtual void serialize_( Simian::FileObjectSubNode& node ) = 0;
        virtual void deserialize_( const Simian::FileObjectSubNode& node ) = 0;
    public:
        AISkillBase( AIEffectType type );
        ~AISkillBase();
        void Serialize( Simian::FileObjectSubNode& node );
        void Deserialize( const Simian::FileObjectSubNode& node );
        virtual bool Update( Simian::f32 dt ) = 0;
    };
}
#endif
