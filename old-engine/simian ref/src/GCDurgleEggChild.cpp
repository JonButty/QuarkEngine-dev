/******************************************************************************/
/*!
\file		GCDurgleEggChild.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCDurgleEggChild.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

#include "Simian/EnginePhases.h"
#include "Simian/CModel.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Delegate.h"
#include "Simian/LogManager.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCDurgleEggChild> GCDurgleEggChild::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_DURGLEEGGCHILD);

    void GCDurgleEggChild::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if(start_)
        {
            Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
            timer_ += dt;

            Simian::f32 intp = timer_/maxTime_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            trans_->Scale(Simian::Interpolation::Interpolate
                <Simian::Vector3>(Simian::Vector3::Zero,finalScale_,intp,Simian::Interpolation::EaseSquareOut));
            
            if(timer_ > maxTime_)
            {
                model_->Visible(false);
                start_ = false;
            }
        }
    }

    //-Public-------------------------------------------------------------------

    void GCDurgleEggChild::Start()
    {
        start_ = true;
        timer_ = 0.0f;
        model_->Visible(true);
        trans_->Scale(Simian::Vector3::Zero);
    }

    //-Public-------------------------------------------------------------------

    GCDurgleEggChild::GCDurgleEggChild()
        :   model_(0),
        trans_(0),
        start_(false),
        maxTime_(1),
        timer_(0),
        scaleIncrease_(Simian::Vector3::Zero),
        finalScale_(Simian::Vector3::Zero)
    {

    }

    GCDurgleEggChild::~GCDurgleEggChild()
    {

    }

    void GCDurgleEggChild::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCDurgleEggChild, &GCDurgleEggChild::update_>(this));
    }

    void GCDurgleEggChild::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        ParentEntity()->ComponentByType(Simian::C_MODEL, model_);
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM,trans_);

        finalScale_ = Simian::Vector3(1,1,1);
        scaleIncrease_ = finalScale_/maxTime_;
        trans_->Scale(Simian::Vector3::Zero);
        model_->Visible(false);
    }

    void GCDurgleEggChild::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("MaxTime", maxTime_);
    }

    void GCDurgleEggChild::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("MaxTime",maxTime_, maxTime_);
    }
}
