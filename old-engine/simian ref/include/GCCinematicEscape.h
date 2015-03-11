/*************************************************************************/
/*!
\file		GCCinematicEscape.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCCINEMATICESCAPE_H_
#define DESCENSION_GCCINEMATICESCAPE_H_

#include "GCInputListener.h"
#include "Simian/FiniteStateMachine.h"

#include <string>

namespace Descension
{
    class GCCinematicEscape: public GCInputListener
    {
    private:
        std::string triggerTarget_;
        Simian::Entity *backBtn_;
        //--static
        static bool canEscape_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCCinematicEscape> factoryPlant_;
    public:
        static void CanEscape(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCCinematicEscape();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void OnTriggered(Simian::DelegateParameter& param);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
