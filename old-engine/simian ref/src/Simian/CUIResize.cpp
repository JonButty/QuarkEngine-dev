/************************************************************************/
/*!
\file		CUIResize.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CUIResize.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/CTransform.h"

namespace Simian
{
    GraphicsDevice* CUIResize::device_ = 0;

    FactoryPlant<EntityComponent, CUIResize> CUIResize::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_UIRESIZE);

    CUIResize::CUIResize()
        : transform_(0),
          majorAxis_(A_HEIGHT),
          positionScale_(PS_PROPORTIONAL),
          size_(0.0f, 0.0f, 1.0f)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& CUIResize::Position() const
    {
        return position_;
    }

    void CUIResize::Position( const Simian::Vector3& val )
    {
        position_ = val;
    }

    const Simian::Vector3& CUIResize::Size() const
    {
        return size_;
    }

    void CUIResize::Size( const Simian::Vector3& val )
    {
        size_ = val;
    }

    CUIResize::Axis CUIResize::MajorAxis() const
    {
        return majorAxis_;
    }

    void CUIResize::MajorAxis( Axis val )
    {
        majorAxis_ = val;
    }

    void CUIResize::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    //--------------------------------------------------------------------------

    void CUIResize::update_( DelegateParameter& )
    {
        f32 axisLength = majorAxis_ == A_WIDTH ? device_->Size().X() :
                         majorAxis_ == A_HEIGHT ? device_->Size().Y() :
                         device_->Size().Y();

        Vector3 axisScales = positionScale_ == PS_PROPORTIONAL ?
            Vector3(axisLength, axisLength, axisLength) : 
            Vector3(device_->Size().X(), device_->Size().Y(), 1.0f);
        
        Simian::Vector3 position = position_ * axisScales;
        position.Z(position_.Z());
        transform_->Position(position);

        Simian::Vector3 size = size_ * axisLength;
        size.Z(size.Z() == 0.0f ? 1.0f : size.Z());
        transform_->Scale(size);
    }

    //--------------------------------------------------------------------------

    void CUIResize::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CUIResize, &CUIResize::update_>(this));
    }

    void CUIResize::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);

		update_(Simian::Delegate::NoParameter);
		transform_->RecomputeWorld();
    }

    void CUIResize::Serialize( FileObjectSubNode& data )
    {
        Simian::FileObjectSubNode* position = data.AddObject("Position");
        position->AddData("X", position_.X());
        position->AddData("Y", position_.Y());
        position->AddData("Z", position_.Z());

        Simian::FileObjectSubNode* size = data.AddObject("Size");
        size->AddData("Width", size_.X());
        size->AddData("Height", size_.Y());
        size->AddData("Depth", size_.Z());

        data.AddData("MajorAxis", majorAxis_ == A_WIDTH ? "Width" :
                                  majorAxis_ == A_HEIGHT ? "Height" :
                                  "Height");

        data.AddData("PositionScale", positionScale_ == PS_PROPORTIONAL ? "Proportional" :
                                      positionScale_ == PS_FREE ? "Free" :
                                      "Proportional");
    }

    void CUIResize::Deserialize( const FileObjectSubNode& data )
    {
        const Simian::FileObjectSubNode* position = data.Object("Position");
        if (position)
        {
            float x, y, z;
            position->Data("X", x, position_.X());
            position->Data("Y", y, position_.Y());
            position->Data("Z", z, position_.Z());
            position_ = Simian::Vector3(x, y, z);
        }

        const Simian::FileObjectSubNode* size = data.Object("Size");
        if (size)
        {
            float x, y, z;
            size->Data("Width", x, size_.X());
            size->Data("Height", y, size_.Y());
            size->Data("Depth", z, size_.Z());
            size_ = Simian::Vector3(x, y, z);
        }

        std::string majorAxis;
        data.Data("MajorAxis", majorAxis, "");
        majorAxis_ = majorAxis == "Width" ? A_WIDTH :
                     majorAxis == "Height" ? A_HEIGHT :
                     majorAxis_;

        std::string positionScale;
        data.Data("PositionScale", positionScale, "");
        positionScale_ = positionScale == "Proportional" ? PS_PROPORTIONAL : 
                         positionScale == "Free" ? PS_FREE :
                         positionScale_;
    }
}
