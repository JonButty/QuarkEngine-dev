/*************************************************************************/
/*!
\file		GCGameOver.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#ifndef DESCENSION_GCGAMEOVER_H_
#define DESCENSION_GCGAMEOVER_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CSprite;
    class Material;
    class ShaderFloatParameter;
}

namespace Descension
{
    class GCGameOver: public Simian::EntityComponent
    {
    private:
        Simian::CSprite* title_;
        Simian::CSprite* revive_;
        Simian::CSprite* background_;

        Simian::Material* healthCriticalMaterial_;
        Simian::ShaderFloatParameter* redness_;

        Simian::f32 timer_;
        bool activated_;
        Simian::f32 radius_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCGameOver> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCGameOver();

        void OnSharedLoad();
        void OnAwake();

        void Show();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
