/******************************************************************************/
/*!
\file		GCDescendingWall.h
\author 	Gavin Yeo, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCDESCENDINGWALL_H_
#define DESCENSION_GCDESCENDINGWALL_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class CSoundEmitter;
}

namespace Descension
{
    
    class GCDescendingWall : public Simian::EntityComponent
    {
	private:
        Simian::CTransform* transform_;
        Simian::CModel* model_;

        Simian::f32 descendSpeed_;
        Simian::f32 timer_;
        Simian::f32 subtitleTimer_;
        Simian::f32 maxTimer_;
        Simian::f32 subtitleStartDelay_;
        Simian::f32 startY_;
        Simian::f32 endY_;
        Simian::s32 currentSubtitle_;
        Simian::f32 dustTimer_, dustTimerMax_;
        bool startDescend_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCDescendingWall> factoryPlant_;
    public:
        Simian::CTransform& Transform();

        bool StartDescend();
        void StartDescend(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCDescendingWall();

        void OnSharedLoad();
        void OnAwake();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
