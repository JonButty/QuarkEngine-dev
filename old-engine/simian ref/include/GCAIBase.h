/******************************************************************************/
/*!
\file		GCAIBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCAIBASE_H_
#define DESCENSION_GCAIBASE_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Vector3.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/EntityComponent.h"

#include "AIEffectBase.h"
#include "AISkillBase.h"

#include <vector>

namespace Simian
{
	class CTransform;
	class CModel;
}
 
namespace Descension
{
	class GCPhysicsController;
	class GCAttributes;

	class GCAIBase : public Simian::EntityComponent
	{
	protected:
        bool overRide_, effectsOn_, effectsOnBirth_, aiPassive_, seekInCine_;
        Simian::CTransform* transform_;
		Simian::CModel* model_;
		GCPhysicsController* physics_;
        AISkillBase* currentSkill_;
        Simian::u32 chanceValue_;
		Simian::s32 totalStates_;
        Simian::Vector3 target_;
		Simian::FiniteStateMachine fsm_;
		std::vector<AIEffectBase*>* masterEffectList_;
		std::vector<AIEffectBase*> effectList_;
        std::vector<AIEffectBase*> skillList_;
		static std::vector<GCAIBase*> allEnemiesVec_;
        std::vector< std::pair<std::string, std::string> > attachments_;

        Simian::u32 aiType_;

		// Deserialized data, attributes of AI entity
		GCAttributes* attributes_;
        GCAttributes* playerAttribs_;
	public:

        enum AI_TYPE
        {
            AI_AGGRESSIVE = 0,
            AI_PASSIVE,

            AI_TYPE_TOTAL
        };

        Simian::u32 AIType(void);
        bool OverRide() const;
        void OverRide(bool val);
        bool AiPassive() const;
        void AiPassive(bool val);
        bool SeekInCine() const;
        void SeekInCine(bool val);
		Simian::CTransform* Transform() const;
		Simian::CModel* Model() const;
		GCPhysicsController* Physics() const;
		Simian::s32 Health() const;
		void MoveSpeed(Simian::f32 val);
		Simian::f32 MoveSpeed() const;
		void Health(Simian::s32 val);
		Simian::FiniteStateMachine& Fsm();
		GCAttributes* Attributes();
		static std::vector<GCAIBase*>& AllEnemiesVec();
	private:
		GCAIBase();
		virtual void update_(Simian::DelegateParameter& param);
        virtual void overRideStateUpdate_( Simian::f32 dt ) = 0;
	public:
		GCAIBase(const Simian::s32& start, const Simian::s32& total);
        virtual ~GCAIBase();
		void OnSharedLoad();
		void OnSharedUnload();
		void Deserialize( const Simian::FileObjectSubNode& data );
		void Serialize( Simian::FileObjectSubNode& data );
		void OnAwake();
		void OnInit();
		void OnDeinit();
        void OverRideBehavoir( const Simian::Vector3& target );
        void ResumeBehavoir();
        Simian::f32 Radius();
	};
}

#endif
