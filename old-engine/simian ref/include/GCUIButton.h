/*************************************************************************/
/*!
\file		GCUIButton.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCUIBUTTON_H_
#define DESCENSION_GCUIBUTTON_H_

#include "GCTrigger.h"

namespace Simian
{
    class Material;
    class CTransform;
    class CSprite;
    class CSoundEmitter;
}

namespace Descension
{
    class GCUIButton: public GCTrigger
    {
    private:
        std::string materialFileNormal_;
        std::string materialFileHover_;

        Simian::Material* normal_;
        Simian::Material* hover_;

        Simian::CTransform* transform_;
        Simian::CSprite* sprite_;
        Simian::CSoundEmitter* sounds_;

        Simian::Vector2 hitArea_;
        bool onHover_;
        bool enabled_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCUIButton> factoryPlant_;
    public:
        bool Enabled() const;
        void Enabled(bool val);

        bool OnHover() const { return onHover_; }
    private:
        void update_(Simian::DelegateParameter&);
    public:
        GCUIButton();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
