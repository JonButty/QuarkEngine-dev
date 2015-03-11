/*************************************************************************/
/*!
\file		GCCursor.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_CURSOR_H_
#define DESCENSION_CURSOR_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CSprite;
};

namespace Descension
{
    class GCMousePlaneIntersection;

    class GCCursor : public Simian::EntityComponent
    {
    private:
        Simian::CTransform* transform_;
        Simian::f32 offsetX_, offsetY_;
        GCMousePlaneIntersection* spirit_;
        Simian::CSprite* sprite_;
        bool showBoth_;

        static GCCursor* instance_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCCursor> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter&);
    public:
        GCCursor();

        static GCCursor* Instance(void);

        GCMousePlaneIntersection* Spirit(void) const;
        void ShowBoth(bool showBoth);

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
