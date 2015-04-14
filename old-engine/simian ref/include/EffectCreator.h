/******************************************************************************/
/*!
\file		EffectCreator.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_EFFECTCREATOR_H_
#define DESCENSION_EFFECTCREATOR_H_

#include "Simian/TemplateFactory.h"
#include "Simian/SimianTypes.h"
#include "Simian/Singleton.h"
#include "Simian/DefinitionTable.h"

namespace Descension
{
    class AIEffectBase;
    class EffectCreator : public Simian::Singleton<EffectCreator>
    {
    private:
        Simian::u32 totalEffects_;
        Simian::DefinitionTable effectTable_;
        Simian::Factory<AIEffectBase> factory_;
    public:
        Simian::u32 TotalEffects() const;
        const Simian::DefinitionTable& EffectTable() const;
    public:
        EffectCreator();
        void CreateInstance(Simian::u32 type, AIEffectBase*& effect);
        void DestroyInstance(AIEffectBase*& instance);
        void Init(Simian::u32 totalEffects, const std::vector<Simian::DataLocation>& effects);
        void CopyInstance(Simian::u32 type, AIEffectBase*& destination, AIEffectBase* source);

        friend class AIEffectBase;
    };
}

#endif
