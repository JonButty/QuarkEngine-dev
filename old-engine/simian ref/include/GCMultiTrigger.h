/*************************************************************************/
/*!
\file		GCMultiTrigger.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCMULTITRIGGER_H_
#define DESCENSION_GCMULTITRIGGER_H_

#include "GCInputListener.h"

#include <vector>
#include <string>

namespace Descension
{
    class GCEventScript;

    class GCMultiTrigger: public GCInputListener
    {
    private:
        std::vector<std::string> triggerList_;
        std::vector<GCEventScript*> triggerPtr_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCMultiTrigger>
            factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCMultiTrigger();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void OnTriggered(Simian::DelegateParameter&);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
