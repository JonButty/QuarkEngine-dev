/******************************************************************************/
/*!
\file		EffectCreator.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "EffectCreator.h"
#include "AIEffectTypes.h"
#include "AIEffectBase.h"

namespace Descension
{
    EffectCreator::EffectCreator()
        :   factory_(EFF_TOTAL)
    {
    }

    //-Public-------------------------------------------------------------------

    Simian::u32 EffectCreator::TotalEffects() const
    {
        return totalEffects_;
    }

    const Simian::DefinitionTable& EffectCreator::EffectTable() const
    {
        return effectTable_;
    }

    //-Public-------------------------------------------------------------------

    void EffectCreator::CreateInstance( Simian::u32 type, AIEffectBase*& effect )
    {
        factory_.CreateInstance(type, effect);
    }

    void EffectCreator::DestroyInstance( AIEffectBase*&  instance)
    {
        factory_.DestroyInstance(instance);
    }

    void EffectCreator::Init( Simian::u32 totalEffects, const std::vector<Simian::DataLocation>& effects )
    {
        totalEffects_ = totalEffects;
        effectTable_.Load(effects);
    }

    void EffectCreator::CopyInstance( Simian::u32 type, AIEffectBase*& destination, AIEffectBase* source)
    {
        factory_.CopyInstance(type, destination, source);
    }
}
