/*************************************************************************/
/*!
\file		GCEditorScene.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCEditor.h"
#include "Collision.h"

#include "Simian/Ray.h"
#include "Simian/Plane.h"
#include "Simian/Mouse.h"
#include "Simian/CTransform.h"
#include "Simian/CCamera.h"
#include "Simian/Camera.h"
#include "Simian/Entity.h"
#include "Simian/CModel.h"
#include "Simian/Scene.h"
#include "Simian/EngineComponents.h"

namespace Descension
{
    void GCEditor::sceneModeEnter_( Simian::DelegateParameter& )
    {

    }

    void GCEditor::sceneModeUpdate_( Simian::DelegateParameter& /* param */ )
    {
        // we might need this some day...
        /*float dt = param.As<Simian::FiniteStateUpdateParameter>().Dt;
        dt;*/

        if (rotating_)
            rotateSelectedObject_();
        if (yTranslate_)
            moveYSelectedObject_();
        else
            moveSelectedObject_();
    }

    void GCEditor::sceneModeExit_( Simian::DelegateParameter& )
    {

    }

    void GCEditor::sceneModeLButtonStart_()
    {
        if (altModifier_)
            rotating_ = true;
        else if (clickedSelectedObject_())
            beginMoveSelectedObject_();
        else
        {
            updatePicking_(shiftModifier_);
            if (selectedObject_)
                beginMoveSelectedObject_();
        }
    }

    void GCEditor::sceneModeLButtonEnd_()
    {
        if (movingSelectedObject_)
            movingSelectedObject_ = false;
        if (rotating_)
            rotating_ = false;
    }

    void GCEditor::sceneModeRButtonStart_()
    {

    }

    void GCEditor::sceneModeRButtonEnd_()
    {

    }

    void GCEditor::computeOffset_()
    {
        Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
            camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));

        for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
        {
            Simian::Plane movePlane(
                Simian::Vector3(0, selectedTransform_[i]->World().Position().Y(), 0),
                Simian::Vector3::UnitY);

            Simian::Vector3 intersection;
            if (movePlane.Intersect(pickingRay, intersection))
            {
                Simian::Vector3 offset = selectedTransform_[i]->World().Position() - intersection;
                groupOffset_.push_back(offset);
            }
        }
    }

    bool GCEditor::clickedSelectedObject_()
    {
        if (!selectedObject_)
            return false;

        Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
            camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));

        for (Simian::u32 i = 0; i < selectedModel_.size(); ++i)
        {
            Simian::Vector3 min, max;
            computeAABB_(min, max, selectedModel_[i], selectedTransform_[i]);

            // check intersection with the aabb
            if (Collision::RayAABB(pickingRay, min, max))
                return true;
        }

        return false;
    }

    static Simian::f32 intervalRound_(Simian::f32 num, Simian::f32 interval)
    {
        return std::floor(num/interval + 0.5f) * interval;
    }

    void GCEditor::moveSelectedObject_()
    {
        if (movingSelectedObject_)
        {
            // find intersection of picking ray with current object's transform (y plane)
            Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
                camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));

            for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
            {
                Simian::Plane movePlane(
                    Simian::Vector3(0, selectedTransform_[i]->World().Position().Y(), 0),
                    Simian::Vector3::UnitY);

                Simian::Vector3 intersection;
                if (movePlane.Intersect(pickingRay, intersection))
                {
                    intersection += groupOffset_[i];

                    // clamp intersection to snap
                    intersection.X(intervalRound_(intersection.X(), xyzSnap_.X()));
                    intersection.Z(intervalRound_(intersection.Z(), xyzSnap_.Z()));
                    if (selectedTransform_[i]->ParentTransform())
                        selectedTransform_[i]->ParentTransform()->World().Inversed().Transform(intersection);
                    selectedTransform_[i]->Position(intersection);
                }
            }
        }
    }

    void GCEditor::beginMoveSelectedObject_()
    {
        // check if c is pressed
        if (cModifier_)
            cloneSelectedObjects_();
        
        yTranslate_ = ctrlModifier_;

        groupOffset_.clear();

        // check if we are translating on the Y
        if (yTranslate_)
            computeYOffset_();
        else
            computeOffset_();

        movingSelectedObject_ = true;
    }

    void GCEditor::cloneSelectedObjects_()
    {
        // clone and remove selected objects
        std::vector<Simian::Entity*> clones;
        for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
        {
            Simian::Entity* entity = selectedTransform_[i]->ParentEntity()->Clone();
            clones.push_back(entity);

            // put it to the same parent
            if (selectedTransform_[i]->ParentEntity()->ParentEntity())
                selectedTransform_[i]->ParentEntity()->ParentEntity()->AddChild(entity);
            else
                ParentScene()->AddEntity(entity);
        }

        selectedModel_.clear();
        selectedTransform_.clear();

        // fill with the new clones
        for (Simian::u32 i = 0; i < clones.size(); ++i)
        {
            Simian::CModel* model;
            Simian::CTransform* transform;
            clones[i]->ComponentByType(Simian::C_MODEL, model);
            clones[i]->ComponentByType(Simian::C_TRANSFORM, transform);

            selectedModel_.push_back(model);
            selectedTransform_.push_back(transform);
        }
    }

    void GCEditor::computeYOffset_()
    {
        Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
            camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));

        Simian::Plane movePlane(
            Simian::Vector3(
            selectedTransform_[0]->World().Position().X(), 0, 
            selectedTransform_[0]->World().Position().Z()),
            -forward_);

        for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
        {
            const Simian::Vector3& position = selectedTransform_[i]->World().Position();

            Simian::Vector3 intersection;
            if (movePlane.Intersect(pickingRay, intersection))
            {
                intersection.X(position.X());
                intersection.Z(position.Z());
                Simian::Vector3 offset = selectedTransform_[i]->World().Position() - intersection;
                groupOffset_.push_back(offset);
            }
        }
    }

    void GCEditor::moveYSelectedObject_()
    {
        if (movingSelectedObject_)
        {
            // find intersection of picking ray with current object's transform (y plane)
            Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
                camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));


            Simian::Plane movePlane(
                Simian::Vector3(
                selectedTransform_[0]->World().Position().X(), 0, 
                selectedTransform_[0]->World().Position().Z()),
                -forward_);

            for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
            {
                const Simian::Vector3& position = selectedTransform_[i]->World().Position();

                Simian::Vector3 intersection;
                if (movePlane.Intersect(pickingRay, intersection))
                {
                    intersection += groupOffset_[i];

                    // clamp intersection to snap
                    intersection.X(position.X());
                    intersection.Z(position.Z());

                    // only change the y
                    intersection.Y(intervalRound_(intersection.Y(), xyzSnap_.Y()));
                    if (selectedTransform_[i]->ParentTransform())
                        selectedTransform_[i]->ParentTransform()->World().Inversed().Transform(intersection);
                    selectedTransform_[i]->Position(intersection);
                }
            }
        }
    }

    void GCEditor::rotateSelectedObject_()
    {
        if (rotating_)
        {
            // rotating the object intersect with z plane
            Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
                camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));

            for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
            {
                Simian::Plane rotatePlane(
                    Simian::Vector3(0, selectedTransform_[i]->World().Position().Y(), 0),
                    Simian::Vector3::UnitY);

                Simian::Vector3 intersection;
                if (rotatePlane.Intersect(pickingRay, intersection))
                {
                    // find the angle on the intersection
                    Simian::Vector3 dirVec = intersection - selectedTransform_[i]->World().Position();
                    dirVec.Y(0);
                    dirVec.Normalize();

                    // get the arccos
                    Simian::f32 angle = std::acos(dirVec.X());
                    if (dirVec.Z() > 0)
                        angle = Simian::Math::TAU - angle;
                    angle = Simian::Radian(angle).Degrees();
                    angle = intervalRound_(angle, rotationSnap_);

                    selectedTransform_[i]->Rotation(selectedTransform_[i]->Axis(),
                        Simian::Degree(angle));
                }
            }
        }
    }
}
