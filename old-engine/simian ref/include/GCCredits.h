/*************************************************************************/
/*!
\file		GCCredits.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCCREDITS_H_
#define DESCENSION_GCCREDITS_H_

#include "GCInputListener.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCCredits: public GCInputListener
    {
    private:
        Simian::CTransform* transform_;
        Simian::Delegate closeCallback_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCCredits> factoryPlant_;
    public:
        void CloseCallback(const Simian::Delegate& val);
    private:
        void registerButton_(const std::string& name);
    public:
        GCCredits();

        void OnAwake();

        void OnTriggered( Simian::DelegateParameter& );

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
