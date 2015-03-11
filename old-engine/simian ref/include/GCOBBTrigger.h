/*************************************************************************/
/*!
\file		GCOBBTrigger.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCOBBTRIGGER_H_
#define DESCENSION_GCOBBTRIGGER_H_

#include "Simian/Vector2.h"
#include "GCTrigger.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCOBBTrigger: public GCTrigger
    {
    private:
        Simian::Vector2 size_;
        bool repeat_;
        bool entered_;
        bool inside_;
        Simian::CTransform* transform_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCOBBTrigger> factoryPlant_;
    public:
        const Simian::Vector2& Size() const;
        void Size(const Simian::Vector2& val);

        bool Repeat() const;
        void Repeat(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCOBBTrigger();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Reset();

        void Deserialize(const Simian::FileObjectSubNode& data);
        void Serialize( Simian::FileObjectSubNode& data );
    };
}

#endif
