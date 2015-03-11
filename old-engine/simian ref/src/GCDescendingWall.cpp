/*************************************************************************/
/*!
\file		GCDescendingWall.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/AnimationController.h"
#include "Simian/CModel.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCDescendingWall.h"
#include "GCEditor.h"
#include "GCCinematicUI.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCDescendingWall> GCDescendingWall::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_DESCENDINGWALL);

    GCDescendingWall::GCDescendingWall()
        : transform_(0),
          model_(0),
          descendSpeed_(0),
          timer_(0),
          subtitleTimer_(0),
          maxTimer_(10.0f),
          dustTimerMax_(5.0f),
          dustTimer_(dustTimerMax_),
          subtitleStartDelay_(5.0f),
          startY_(-0.1f),
          endY_(-2.0f),
          currentSubtitle_(10),
          startDescend_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCDescendingWall::Transform()
    {
        return *transform_;
    }

    bool GCDescendingWall::StartDescend()
    {
        return startDescend_;
    }
    
    void GCDescendingWall::StartDescend(bool val)
    {
        if (!startDescend_ && val)
        {
            timer_ = 0.0f;
            model_->Controller().PlayAnimation("Shake", true);
        }
        else if (!val)
            model_->Controller().PlayAnimation("Idle", false);

        startDescend_ = val;
    }

    //--------------------------------------------------------------------------

    void GCDescendingWall::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        if (startDescend_ && timer_ < maxTimer_)
        {
            timer_ += dt;
            Simian::f32 intp = timer_/maxTimer_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            float speed = (endY_-startY_)/maxTimer_;
            float currentY = transform_->Position().Y()+(speed*dt);
            
            transform_->Position(Simian::Vector3(
                        transform_->Position().X(), currentY, transform_->Position().Z()));

            if (timer_ >= subtitleStartDelay_)
            {
                subtitleTimer_ += dt;
                if (subtitleTimer_ >= 1.0f)
                {
                    subtitleTimer_ = 0.0f;
                    --currentSubtitle_;
                    std::stringstream tempStream;
                    tempStream << currentSubtitle_;
                    //GCCinematicUI::Instance().ShowSubtitle(tempStream.str(), 2.0f);
                }
            }

            dustTimer_ += dt;
            if (dustTimer_ > dustTimerMax_)
            {
                dustTimer_ = 0.0f;
                Simian::Entity* dust = ParentEntity()->ParentScene()->CreateEntity(E_QUAKEDUST);
		        ParentEntity()->ParentScene()->AddEntity(dust);
		        // Set transform
		        Simian::CTransform* dustTransform_;
		        dust->ComponentByType(Simian::C_TRANSFORM, dustTransform_);
		        dustTransform_->Position(Simian::Vector3(transform_->World().Position().X()-0.25f,
			        transform_->World().Position().Y()+0.3f, transform_->World().Position().Z()+0.25f));

                Simian::Entity* dust2 = ParentEntity()->ParentScene()->CreateEntity(E_QUAKEDUST);
		        ParentEntity()->ParentScene()->AddEntity(dust2);
		        // Set transform
		        dust2->ComponentByType(Simian::C_TRANSFORM, dustTransform_);
		        dustTransform_->Position(Simian::Vector3(transform_->World().Position().X()-0.25f,
			        transform_->World().Position().Y()+0.3f, transform_->World().Position().Z()-0.25f));
            }
        }
        else
            startDescend_ = false;
    }

    //--------------------------------------------------------------------------

    void GCDescendingWall::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCDescendingWall, &GCDescendingWall::update_>(this));
    }

    void GCDescendingWall::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_MODEL, model_);

        transform_->Position(Simian::Vector3(
                        transform_->Position().X(), startY_, transform_->Position().Z()));
        currentSubtitle_ = static_cast<Simian::s32>(maxTimer_-subtitleStartDelay_);

#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (startDescend_)
            model_->Controller().PlayAnimation("Shake", true);
    }

    void GCDescendingWall::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("MaxTimer", maxTimer_);
        data.AddData("DustMaxTimer", dustTimerMax_);
        data.AddData("SubtitlesStartDelay", subtitleStartDelay_);
        data.AddData("StartDescend", startDescend_);
        data.AddData("StartY", startY_);
        data.AddData("EndY", endY_);
    }

    void GCDescendingWall::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the data
        data.Data("MaxTimer", maxTimer_, maxTimer_);
        data.Data("DustMaxTimer", dustTimerMax_, dustTimerMax_);
        data.Data("SubtitlesStartDelay", subtitleStartDelay_, subtitleStartDelay_);
        data.Data("StartDescend", startDescend_, startDescend_);
        data.Data("StartY", startY_, startY_);
        data.Data("EndY", endY_, endY_);
    }
}
