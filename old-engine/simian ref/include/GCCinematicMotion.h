/******************************************************************************/
/*!
\file		GCCinematicMotion.h
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCCINEMATICMOTION_H_
#define DESCENSION_GCCINEMATICMOTION_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/Angle.h"

namespace Simian
{
    class CTransform;
    class CUIResize;
}

namespace Descension
{
	class GCCinematicMotion: public Simian::EntityComponent
	{
	private:
		Simian::f32 initialAngle_;
		Simian::f32 finalAngle_;
		Simian::f32 angleTimer_;
		Simian::f32 angleDuration_;

		Simian::Vector3 initialPosition_;
		Simian::Vector3 finalPosition_;
		Simian::f32 movementTimer_;
		Simian::f32 movementDuration_;

        bool enabled_;

        Simian::CTransform* transform_;
        Simian::CUIResize* ui_;

        void(GCCinematicMotion::*setPosition_)(const Simian::Vector3&);
        Simian::Vector3(GCCinematicMotion::*getPosition_)();

        static Simian::FactoryPlant<Simian::EntityComponent, GCCinematicMotion> factoryPlant_;
	private:
		void update_(Simian::DelegateParameter&);

        Simian::Vector3 uiPosition_();
        void uiPosition_(const Simian::Vector3& pos);

        Simian::Vector3 transformPosition_();
        void transformPosition_(const Simian::Vector3& pos);
	public:
		GCCinematicMotion();

		void OnSharedLoad();
		void OnAwake();

		void Rotate(const Simian::Angle& target, Simian::f32 duration);
		void Translate(const Simian::Vector3& target, Simian::f32 duration);

		void Serialize(Simian::FileObjectSubNode&);
		void Deserialize(const Simian::FileObjectSubNode&);
	};
}

#endif
