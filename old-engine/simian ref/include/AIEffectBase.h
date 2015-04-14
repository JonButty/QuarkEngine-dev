/******************************************************************************/
/*!
\file		AIEffectBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTBASE_H_
#define DESCENSION_AIEFFECTBASE_H_

#include "Simian/FileObjectSubNode.h"
#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/TemplateFactory.h"

#include "EffectCreator.h"
#include "AIEffectTypes.h"
 
namespace Descension
{
	class GCAttributes;

	class AIEffectBase
	{
	protected:
        bool skill_;
        bool enable_;
        bool onEnter_;
		bool loaded_;
        Descension::AIEffectType type_;
		GCAttributes* target_;
	public:
        bool Skill() const;
        void Skill(bool val);
		bool Enable() const;
		void Enable(bool val);
		bool OnEnter() const;
		void OnEnter(bool val);
		Descension::AIEffectType Type() const;
		void Type(Descension::AIEffectType val);
		bool Loaded() const;
		GCAttributes* Target() const;
	protected:
        virtual void onAwake_() = 0;
		virtual void onInit_( GCAttributes* source)=0;
		virtual void onUnload_()=0;
        virtual void serialize_( Simian::FileObjectSubNode& node ) = 0;
        virtual void deserialize_( const Simian::FileObjectSubNode& node ) = 0;
		template <class T>
		static Simian::FactoryPlant<AIEffectBase, T> createFactoryPlant_(Simian::s32 type)
		{
			return Simian::FactoryPlant<AIEffectBase, T>(&EffectCreator::Instance().factory_, type);
		}
	public:
        AIEffectBase( AIEffectType type );
        virtual ~AIEffectBase();
        void OnAwake();
		void OnInit( GCAttributes* target, GCAttributes* source = 0 );
		void OnUnload();
		virtual bool Update( Simian::f32 dt ) = 0;
        virtual void Serialize( Simian::FileObjectSubNode& node );
        virtual void Deserialize( const Simian::FileObjectSubNode& node );
	};
}

#endif
