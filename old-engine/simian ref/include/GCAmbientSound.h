/*************************************************************************/
/*!
\file		GCAmbientSound.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#ifndef DESCENSION_GCAMBIENTSOUND_H_
#define DESCENSION_GCAMBIENTSOUND_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CSoundEmitter;
}

namespace Descension
{
    class GCAmbientSound: public Simian::EntityComponent
    {
    private:
        Simian::CSoundEmitter* emitter_;

        Simian::u32 soundId_;
        Simian::f32 timer_;
        Simian::f32 interval_;
        Simian::f32 randomInterval_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAmbientSound> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCAmbientSound();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
