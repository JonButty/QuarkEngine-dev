/*************************************************************************/
/*!
\file		GCTrigger.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCTRIGGGER_H_
#define DESCENSION_GCTRIGGGER_H_

#include "Simian/EntityComponent.h"

#include <string>
#include <vector>

namespace Descension
{
    class GCInputListener;

    class GCTrigger: public Simian::EntityComponent
    {
    private:
        struct TalTarget
        {
            std::string targetName_;
            std::string listenerName_;
            GCInputListener* listener_;
        };

        std::vector<TalTarget> tals_;
    public:
        GCTrigger();

        void OnAwake();

        void Trigger(void* data = 0);

        void AddListener(GCInputListener* listener);

        virtual void Serialize( Simian::FileObjectSubNode& data );
        virtual void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
