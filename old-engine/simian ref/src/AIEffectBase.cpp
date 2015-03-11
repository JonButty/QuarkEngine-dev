/******************************************************************************/
/*!
\file		AIEffectBase.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "AIEffectBase.h"
#include "AIEffectTypes.h"

#include "GCAttributes.h"
#include "GCEditor.h"

namespace Descension
{
    AIEffectBase::AIEffectBase(AIEffectType type)
        :   skill_(false),
            enable_(true),
            onEnter_(true),
            loaded_(false),
            type_(type),
            target_(0)
    {
    }

    AIEffectBase::~AIEffectBase()
    {
    }

    //--------------------------------------------------------------------------

    bool AIEffectBase::Loaded() const
    {
        return loaded_;
    }

    GCAttributes* AIEffectBase::Target() const
    {
        return target_;
    }

    //--------------------------------------------------------------------------

    void AIEffectBase::OnAwake()
    {
        onAwake_();
    }

    void AIEffectBase::OnInit( GCAttributes* target, GCAttributes* source )
    {
        if(target)
        {
            loaded_ = true;
            target_ = target;
            onInit_(source);
        }
        else
            loaded_ = false;
    }

    void AIEffectBase::OnUnload()
    {
        if (!loaded_)
            return;
        onUnload_();
        loaded_ = false;
    }

    //--------------------------------------------------------------------------

    void AIEffectBase::Skill( bool val )
    {
        skill_ = val;
    }

    bool AIEffectBase::Skill() const
    {
        return skill_;
    }

    bool AIEffectBase::Enable() const
    {
        return enable_;
    }

    void AIEffectBase::Enable( bool val )
    {
        enable_ = val;
    }

    bool AIEffectBase::OnEnter() const
    {
        return onEnter_;
    }

    void AIEffectBase::OnEnter( bool val )
    {
        onEnter_ = val;
    }

    Descension::AIEffectType AIEffectBase::Type() const
    {
        return type_;
    }

    void AIEffectBase::Type( Descension::AIEffectType val )
    {
        type_ = val;
    }

    void AIEffectBase::Serialize( Simian::FileObjectSubNode& node )
    {
        node.AddData("Enable",enable_);
        serialize_(node);
    }

    void AIEffectBase::Deserialize( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("Enable");
        enable_ = data? data->AsBool() : enable_;
        deserialize_(node);
    }
}
