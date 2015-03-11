/******************************************************************************/
/*!
\file		GCAIBase.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/EntityComponent.h"
#include "Simian/EnginePhases.h"
#include "Simian/DataLocation.h"
#include "Simian/CModel.h"
#include "Simian/CTransform.h"

#include "GCAIBase.h"
#include "GCPhysicsController.h"
#include "Collision.h"
#include "GCEditor.h"
#include "GCPlayerLogic.h"

#include "EffectCreator.h"
#include "ComponentTypes.h"
#include "GCAttributes.h"

#include <vector>
#include <algorithm>
#include "Simian/LogManager.h"

namespace Descension
{
	static const Simian::u32 EFFECT_INITIAL_SIZE = 8;
	// Vector of all enemies
	std::vector<GCAIBase*> GCAIBase::allEnemiesVec_;

	GCAIBase::GCAIBase(const Simian::s32& start, const Simian::s32& total)
		:   overRide_(false),
            effectsOn_(true),
            effectsOnBirth_(false),
            aiPassive_(false),
            seekInCine_(false),
			transform_(0),
            playerAttribs_(0),
			model_(0),
			physics_(0),
			currentSkill_(0),
			chanceValue_(0),
			totalStates_(total),
			masterEffectList_(0),
			fsm_(start,total),
            aiType_(AI_AGGRESSIVE)
	{
		effectList_.reserve(EFFECT_INITIAL_SIZE);
		skillList_.reserve(EFFECT_INITIAL_SIZE);
	}

	GCAIBase::~GCAIBase()
	{
	}

	//-Public-------------------------------------------------------------------

    Simian::u32 GCAIBase::AIType(void)
    {
        return aiType_;
    }

	bool GCAIBase::OverRide() const
	{
		return overRide_;
	}

	void GCAIBase::OverRide( bool val )
	{
		overRide_ = val;
	}

    bool GCAIBase::AiPassive() const
    {
        return aiPassive_;
    }

    void GCAIBase::AiPassive( bool val )
    {
        aiPassive_ = val;
    }

    bool GCAIBase::SeekInCine() const
    {
        return seekInCine_;
    }

    void GCAIBase::SeekInCine( bool val )
    {
        seekInCine_ = val;
    }

	Simian::CTransform* GCAIBase::Transform() const
	{
		return transform_;
	}

	Simian::CModel* GCAIBase::Model() const
	{
		return model_;
	}

	GCPhysicsController* GCAIBase::Physics() const
	{
		return physics_;
	}

	Simian::FiniteStateMachine& GCAIBase::Fsm()
	{
		return fsm_;
	}

	std::vector<GCAIBase*>& GCAIBase::AllEnemiesVec()
	{
		return allEnemiesVec_;
	}

	GCAttributes* GCAIBase::Attributes()
	{
		return attributes_;
	}

	//-Public-------------------------------------------------------------------

	void GCAIBase::OnSharedLoad()
	{
		RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAIBase, &GCAIBase::update_>(this)); 
	}

	void GCAIBase::OnSharedUnload()
	{
        if (masterEffectList_)
        {
		    for(Simian::u32 i = 0; i < masterEffectList_->size(); ++i)
			    EffectCreator::Instance().DestroyInstance((*masterEffectList_)[i]);
		    delete masterEffectList_;
            masterEffectList_ = 0;
        }

        if (!ParentEntity()->Anonymous())
		    allEnemiesVec_.clear();
	}

	void GCAIBase::Deserialize( const Simian::FileObjectSubNode& data )
	{
		/////////////
		// Effects //
		/////////////
        data.Data("EffectsOnBirth",effectsOnBirth_,effectsOnBirth_);
		if(!masterEffectList_)
		{
			masterEffectList_ = new std::vector<AIEffectBase*>;
			const Simian::FileArraySubNode* effectsArray = data.Array("Effects");

			// Checks if "Effects" node exists
			// Clears the list
			if(effectsArray)
			{
				// Loops through the list of effects and runs their update
				for(Simian::u32 i = 0; i < effectsArray->Size(); ++i)
				{
					const Simian::FileObjectSubNode* effectNode = effectsArray->Object(i);

					// Checks if there is an array of effects
					if(!effectsArray)
						return;
					const Simian::FileDataSubNode* typeNode = effectNode->Data("Type");

					// Checks if there is an effect type defined
					if(!typeNode)
						return;

					// Checks if the type found is on the effects definition table
					if(EffectCreator::Instance().EffectTable().HasValue(typeNode->AsString()))
					{
						Simian::s32 typeId = EffectCreator::Instance().EffectTable().Value(typeNode->AsString());
						AIEffectBase* effect = 0;
						EffectCreator::Instance().CreateInstance(typeId, effect);

						// Checks if an instance of the effect has been created
						if(effect)
						{
							effect->Type(static_cast<AIEffectType>(typeId));
							effect->Deserialize(*effectNode);
							masterEffectList_->push_back(effect);
						}
					}
				}
			}
		}

        // for weapon or other attachments
        const Simian::FileArraySubNode* attachmentsArray = data.Array("Attachments");
		if(attachmentsArray)
		{
            attachments_.clear();
			// Loops through the list of attachments and runs their update
			for(Simian::u32 i = 0; i < attachmentsArray->Size(); ++i)
			{
				const Simian::FileObjectSubNode* attachmentNode = attachmentsArray->Object(i);
				const Simian::FileDataSubNode* jointNode = attachmentNode->Data("Joint");
                const Simian::FileDataSubNode* objNode = attachmentNode->Data("Object");
				// Checks if there is a joint node and object node defined
				if(!jointNode || !objNode)
					break;
                // Push into vector of attachments
                std::pair<std::string, std::string> tempPair(
                                        jointNode->AsString(), objNode->AsString());
                attachments_.push_back(tempPair);
	        }
		}
	}

	void GCAIBase::Serialize( Simian::FileObjectSubNode& data )
    {
        /////////////
        // Effects //
        /////////////

        if (masterEffectList_)
        {
            Simian::FileArraySubNode* effects = data.AddArray("Effects");
            for (Simian::u32 i = 0; i < masterEffectList_->size(); ++i)
            {
                AIEffectBase* effect = (*masterEffectList_)[i];
                std::string type = EffectCreator::Instance().EffectTable().Key(effect->Type());
                if (type == "")
                    continue;

                Simian::FileObjectSubNode* effectNode = effects->AddObject("Effect");
                effectNode->AddData("Type", type);
                effect->Serialize(*effectNode);
            }
        }

        Simian::FileArraySubNode* attachments = data.AddArray("Attachments");
        for(Simian::u32 i = 0; i < attachments_.size(); ++i)
        {
            Simian::FileObjectSubNode* attachment = attachments->AddObject("Attachment");
            attachment->AddData("Joint", attachments_[i].first);
            attachment->AddData("Object", attachments_[i].second);
        }
	}

	void GCAIBase::OnAwake()
	{
		ComponentByType(GC_ATTRIBUTES, attributes_);
		ComponentByType(Simian::C_MODEL, model_);
		ComponentByType(GC_PHYSICSCONTROLLER,physics_);
		allEnemiesVec_.push_back(this);

        attributes_->LogicComponent(this);
        attributes_->StartFromBirth() ? fsm_.ChangeState(attributes_->BirthState()) : 0;
	}

	void GCAIBase::OnInit()
	{
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM, transform_);
        playerAttribs_ = GCPlayerLogic::Instance()->Attributes();
		for(Simian::u32 i = 0; i < masterEffectList_->size(); ++i)
		{
			AIEffectBase* effect = 0;
			EffectCreator::Instance().CreateInstance((*masterEffectList_)[i]->Type(), effect);
			EffectCreator::Instance().CopyInstance((*masterEffectList_)[i]->Type(),effect,(*masterEffectList_)[i]);
			effect->OnInit(attributes_);
			
			if(!effect->Skill()) 
				effectList_.push_back(effect);
			else
				skillList_.push_back(effect);
		}

        // for weapons and other attachments
        if (!attachments_.empty())
            for (Simian::u32 i = 0; i < attachments_.size(); ++i)
                model_->AttachEntityToJoint(attachments_[i].first, ParentEntity()->ChildByName(attachments_[i].second));
	}

	void GCAIBase::OnDeinit()
	{
		for(Simian::u32 i = 0; i < effectList_.size(); ++i)
		{
			effectList_[i]->OnUnload();
			EffectCreator::Instance().DestroyInstance(effectList_[i]);
		}
		effectList_.clear();


        for(Simian::u32 i = 0; i < skillList_.size(); ++i)
        {
            skillList_[i]->OnUnload();
            EffectCreator::Instance().DestroyInstance(skillList_[i]);
        }
        skillList_.clear();
        std::vector<GCAIBase*>::iterator ite = std::find(allEnemiesVec_.begin(), allEnemiesVec_.end(), this);
        if (ite != allEnemiesVec_.end())
            allEnemiesVec_.erase(ite);
	}

	void GCAIBase::update_(Simian::DelegateParameter& param)
	{
#ifdef _DESCENDED
		if (GCEditor::EditorMode())
			return;
#endif
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        if(overRide_)
        {
            overRideStateUpdate_(dt);
            return;
        }

		if(!aiPassive_ && effectsOn_  && attributes_->Health() > 0 )
		{
			for(Simian::u32 i = 0; i < effectList_.size(); ++i)
			{
				if(effectList_[i]->Update(dt))
				{
					Simian::Vector3 vel(attributes_->Direction() * attributes_->MovementSpeed());
					physics_->Velocity(vel);
					return;
				}
			}
		}
		// if all effects return false
		if(GCPlayerLogic::Instance()->Attributes()->Health() < Simian::Math::EPSILON)
            attributes_->ForceSeek(false);
		    fsm_.Update(dt);

        attributes_->CurrentState(fsm_.CurrentState());
		Simian::Vector3 vel(attributes_->Direction() * attributes_->MovementSpeed());
		physics_->Velocity(vel);
	}

	void GCAIBase::OverRideBehavoir( const Simian::Vector3& target )
	{
		overRide_ = true;
		target_ = target;
	}

	void GCAIBase::ResumeBehavoir()
	{
		overRide_ = false;
	}

    Simian::f32 GCAIBase::Radius()
    {
        if(physics_)
            return physics_->Radius();
        return 0;
    }
}
