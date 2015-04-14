/************************************************************************/
/*!
\file		CUIResize.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CUIRESIZE_H_
#define SIMIAN_CUIRESIZE_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Simian
{
    class CTransform;
    class GraphicsDevice;

    class SIMIAN_EXPORT CUIResize: public EntityComponent
    {
    public:
        enum Axis
        {
            A_WIDTH,
            A_HEIGHT
        };

        enum PositionScale
        {
            PS_PROPORTIONAL,
            PS_FREE
        };
    private:
        CTransform* transform_;
        Vector3 position_;
        Vector3 size_;
        Axis majorAxis_;
        PositionScale positionScale_;
        
        static GraphicsDevice* device_;
        static FactoryPlant<EntityComponent, CUIResize> factoryPlant_;
    public:
        const Simian::Vector3& Position() const;
        void Position(const Simian::Vector3& val);

        const Simian::Vector3& Size() const;
        void Size(const Simian::Vector3& val);

        CUIResize::Axis MajorAxis() const;
        void MajorAxis(Axis val);

        static void Device(GraphicsDevice* val);
    private:
        void update_(DelegateParameter& param);
    public:
        CUIResize();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
