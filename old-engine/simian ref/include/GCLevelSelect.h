/*************************************************************************/
/*!
\file		GCLevelSelect.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCLEVELSELECT_H_
#define DESCENSION_GCLEVELSELECT_H_

#include "GCInputListener.h"
#include <vector>

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCLevelSelect: public GCInputListener
    {
    public:
        static Simian::u32 BUTTON_COUNT;

    private:
        Simian::CTransform* transform_;
        Simian::Delegate closeCallback_;

        std::vector<Simian::Entity*>aVec_, bVec_, cVec_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCLevelSelect> factoryPlant_;
    public:
        void CloseCallback(const Simian::Delegate& val);
    private:
        void registerButton_(const std::string& name);
        void update_(Simian::DelegateParameter&);

        void SetAllButtons(void);

        void LoadLevel(const std::string& callee);
    public:
        GCLevelSelect();

        void OnSharedLoad(void);

        void OnAwake();

        void OnTriggered( Simian::DelegateParameter& );

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
