/*************************************************************************/
/*!
\file		GCEditor.cpp
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
#include "ComponentTypes.h"
#include "EntityTypes.h"
#include "VisualTile.h"
#include "TileMap.h"
#include "GCTileMap.h"
#include "Collision.h"
#include "GCPhysicsController.h"
#include "StateEditor.h"

#include "Simian/Game.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/EnginePhases.h"
#include "Simian/RenderQueue.h"
#include "Simian/MaterialCache.h"
#include "Simian/Scene.h"
#include "Simian/CTransform.h"
#include "Simian/Keyboard.h"
#include "Simian/Mouse.h"
#include "Simian/Interpolation.h"
#include "Simian/CCamera.h"
#include "Simian/DebugRenderer.h"
#include "Simian/LogManager.h"
#include "Simian/Math.h"
#include "Simian/Ray.h"
#include "Simian/Plane.h"
#include "Simian/Camera.h"
#include "Simian/CModel.h"
#include "Simian/Model.h"
#include "Simian/CJukebox.h"

#include <math.h>

namespace Descension
{
    static const Simian::f32 AABB_PADDING = 0.1f;
    static const Simian::f32 ZOOM_AMOUNT = 1.0f;
    static const Simian::f32 MIN_ZOOM = -1.0f;
    static const Simian::f32 X_ROTATE_SPEED = 1.0f;
    static const Simian::f32 Y_ROTATE_SPEED = 1.0f;

    GCEditor* GCEditor::instance_ = NULL;
    bool GCEditor::editorMode_ = false;

    Simian::FactoryPlant<Simian::EntityComponent, GCEditor> GCEditor::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_EDITOR);

    Simian::VertexFormat editorVertexFormat_()
    {
        Simian::VertexFormat format;
        format.AddAttribute(Simian::VertexAttribute::VFU_POSITION, Simian::VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(Simian::VertexAttribute::VFU_COLOR, Simian::VertexAttribute::VFT_COLOR);
        return format;
    }
    const Simian::VertexFormat EditorVertex::Format = editorVertexFormat_();

    GCEditor::GCEditor()
        : disabled_(false),
          modeSM_(EM_COLLISION, EM_TOTAL),
          vertexBuffer_(0),
          width_(0),
          height_(0),
          material_(0),
          transform_(0),
          cameraYawTransform_(0),
          cameraPitchTransform_(0),
          cameraNodeTransform_(0),
          camera_(0),
          tileMap_(0),
          terraformMap_(0),
          painted_(false),
          paintingSM_(PE_DEFAULT, PE_TOTAL),
          xyzSnap_(0.5f, 0.5f, 0.5f),
          rotationSnap_(45.0f),
          selectedObject_(false),
          selectedModel_(0),
          selectedTransform_(0),
          movingSelectedObject_(false),
          shiftModifier_(false),
          ctrlModifier_(false),
          altModifier_(false),
          cModifier_(false),
          yTranslate_(false),
          rotating_(false),
          visibility_(255)
    {
        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(*vertexBuffer_);
    }

    GCEditor::~GCEditor()
    {
        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(*vertexBuffer_);
    }

    //--------------------------------------------------------------------------

    GCEditor& GCEditor::Instance()
    {
        return *instance_;
    }

    bool GCEditor::EditorMode()
    {
        return editorMode_;
    }

    void GCEditor::EditorMode( bool mode )
    {
        editorMode_ = mode;
    }

    Simian::FiniteStateMachine& GCEditor::ModeSM()
    {
        return modeSM_;
    }

    Simian::Vector3 GCEditor::XyzSnap() const
    {
        return xyzSnap_;
    }

    void GCEditor::XyzSnap( Simian::Vector3 val )
    {
        xyzSnap_ = val;
    }

    Simian::f32 GCEditor::RotationSnap() const
    {
        return rotationSnap_;
    }

    void GCEditor::RotationSnap( Simian::f32 val )
    {
        rotationSnap_ = val;
    }

    Simian::Entity* GCEditor::SelectedObject() const
    {
        return selectedObject_ ? selectedTransform_[0]->ParentEntity() : 0;
    }

    bool GCEditor::Disabled() const
    {
        return disabled_;
    }

    void GCEditor::Disabled( bool val )
    {
        disabled_ = val;
    }

    Simian::CTransform* GCEditor::CameraBaseTransform() const
    {
        return transform_;
    }

    //--------------------------------------------------------------------------

    void GCEditor::createVisualTiles_()
    {
        // visual tiles for collision
        Simian::Material* material = 
            Simian::Game::Instance().MaterialCache().Load("Materials/VisualTileMaterial.mat");

        // visual tiles for terraforming
        Simian::Material* blueMaterial = 
            Simian::Game::Instance().MaterialCache().Load("Materials/VisualTileMaterialBlue.mat");

        // create the visual tiles to represent the tiles as well
        visualTiles_.resize(width_);
        terraformTiles_.resize(width_);
        for (Simian::u32 i = 0; i < width_; ++i)
        {
            visualTiles_[i].resize(height_);
            terraformTiles_[i].resize(height_);
            for (Simian::u32 j = 0; j < height_; ++j)
            {
                visualTiles_[i][j] = new VisualTile(
                    Simian::Vector2(static_cast<Simian::f32>(i), 
                    static_cast<Simian::f32>(j)), 
                    &(*tileMap_)[i][j], material);
                terraformTiles_[i][j] = new VisualTile(
                    Simian::Vector2(static_cast<Simian::f32>(i), 
                    static_cast<Simian::f32>(j)), 
                    &(*terraformMap_)[i][j], blueMaterial);
            }
        }
    }

    void GCEditor::createGrid_()
    {
        vertexBuffer_->Unload();
        vertexBuffer_->Load(EditorVertex::Format, (width_ + height_ + 2) * 2);

        std::vector<EditorVertex> verts;
        for (Simian::u32 i = 0; i < width_ + 1; ++i)
        {
            verts.push_back(EditorVertex(Simian::Vector3(0, 0, static_cast<Simian::f32>(i)), 
                Simian::Color()));
            verts.push_back(EditorVertex(Simian::Vector3(static_cast<Simian::f32>(height_), 0, static_cast<Simian::f32>(i)), 
                Simian::Color()));
        }
        for (Simian::u32 i = 0; i < height_ + 1; ++i)
        {
            verts.push_back(EditorVertex(
                Simian::Vector3(static_cast<Simian::f32>(i), 0, 0), 
                Simian::Color()));
            verts.push_back(EditorVertex(
                Simian::Vector3(static_cast<Simian::f32>(i), 0, static_cast<Simian::f32>(width_)), 
                Simian::Color()));
        }
        vertexBuffer_->Data(&verts[0]);
    }

    void GCEditor::update_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        acceleration_ = movement_.Z() * forward_ + movement_.X() * side_ 
            + movement_.Y() * Simian::Vector3::UnitY;
        acceleration_ *= CAMERA_SPEED;
        acceleration_ -= velocity_ * CAMERA_DAMP;

        velocity_ += acceleration_ * dt;

        // move camera
        transform_->Position(transform_->Position() + velocity_ * dt);
        StateEditor::Instance().LastCameraPosition(transform_->Position());

        // reset movement vector
        movement_ = Simian::Vector3::Zero;

        // don't run any controls if the editor window is not in focus
        if (disabled_)
            return;

        computeCoordinateFrame_();
        computeMovement_();

        computePickingRay_();
        updateModifiers_();
        updateZoom_();
        updateCameraRotation_();

        modeSM_.Update(dt);
        paintingSM_.Update(dt);
    }

    Simian::Vector3 GCEditor::chooseMin_( const Simian::Vector3& a, const Simian::Vector3& b )
    {
        Simian::Vector3 ret;
        ret.X(a.X() < b.X() ? a.X() : b.X());
        ret.Y(a.Y() < b.Y() ? a.Y() : b.Y());
        ret.Z(a.Z() < b.Z() ? a.Z() : b.Z());
        return ret;
    }

    Simian::Vector3 GCEditor::chooseMax_( const Simian::Vector3& a, const Simian::Vector3& b )
    {
        Simian::Vector3 ret;
        ret.X(a.X() > b.X() ? a.X() : b.X());
        ret.Y(a.Y() > b.Y() ? a.Y() : b.Y());
        ret.Z(a.Z() > b.Z() ? a.Z() : b.Z());
        return ret;
    }

    void GCEditor::render_( Simian::DelegateParameter& )
    {
        if (SIsFlagSet(visibility_, VF_GRID))
            Simian::Game::Instance().RenderQueue().AddRenderObject(
                Simian::RenderObject(*material_, Simian::RenderQueue::RL_SCENE, Simian::Matrix::Identity,
                Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::draw_>(this), NULL));

        // draw the visual tiles
        if (SIsFlagSet(visibility_, VF_COLLISION))
            for (Simian::u32 i = 0; i < visualTiles_.size(); ++i)
                for (Simian::u32 j = 0; j < visualTiles_[i].size(); ++j)
                {
                    visualTiles_[i][j]->Draw();
                    terraformTiles_[i][j]->Draw();
                }

        drawAABB_();
    }

    void GCEditor::updatePicking_(bool addToSelection)
    {
        Simian::Ray pickingRay(
            Simian::Ray::FromProjection(cameraNodeTransform_->World(), 
            camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()));

        Simian::CModel* model = 0;
        Simian::CTransform* transform = 0;

        // loop through all entities
        Simian::f32 t = FLT_MAX;
        updatePicking_(pickingRay, ParentScene()->RootEntities(), model, transform, t);

        if (!addToSelection)
        {
            selectedObject_ = false;
            selectedTransform_.clear();
            selectedModel_.clear();
        }

        if (transform)
        {
            selectedObject_ = true;
            selectedModel_.push_back(model);
            selectedTransform_.push_back(transform);
        }
    }

    void GCEditor::updatePicking_( const Simian::Ray& pickingRay, const std::vector<Simian::Entity*>& entityList, Simian::CModel*& closestModel, Simian::CTransform*& closestTransform, Simian::f32& minDist )
    {
        for (std::vector<Simian::Entity*>::const_iterator i = entityList.begin();
            i != entityList.end(); ++i)
        {
            // ignore camera
            if ((*i)->Name() == "Camera Base")
                continue;

            // try to get model and transform
            Simian::CModel* model;
            Simian::CTransform* transform;

            (*i)->ComponentByType(Simian::C_MODEL, model);
            (*i)->ComponentByType(Simian::C_TRANSFORM, transform);

            if (transform)
            {
                Simian::Vector3 min, max;
                computeAABB_(min, max, model, transform);
                Simian::Vector3 intersection;
                if (Collision::RayAABB(pickingRay, min, max, intersection))
                {
                    Simian::f32 dist = camera_->Camera().Direction().Dot(intersection - cameraNodeTransform_->World().Position());
                    if (dist > 0 && dist < minDist)
                    {
                        minDist = dist;
                        closestModel = model;
                        closestTransform = transform;
                    }
                }
            }

            updatePicking_(pickingRay, (*i)->Children(), closestModel, closestTransform, minDist);
        }
    }

    void GCEditor::computeAABB_( Simian::Vector3& min, Simian::Vector3& max, Simian::CModel* model, Simian::CTransform* transform )
    {
        min = model ? model->Model().Min() : 
            -Simian::Vector3(AABB_PADDING, AABB_PADDING, AABB_PADDING);
        max = model ? model->Model().Max() : 
            Simian::Vector3(AABB_PADDING, AABB_PADDING, AABB_PADDING);
        Simian::Vector3 delta = max - min;

        // find the 8 points of the aabb "cube"
        Simian::Vector3 aabbPoints[] = {
            min + Simian::Vector3(delta.X(), 0, 0),
            min + Simian::Vector3(delta.X(), delta.Y(), 0),
            min + Simian::Vector3(0, delta.Y(), 0),
            min + Simian::Vector3(0, 0, delta.Z()),
            min + Simian::Vector3(delta.X(), 0, delta.Z()),
            min + Simian::Vector3(0, delta.Y(), delta.Z())
        };

        transform->World().Transform(min);
        transform->World().Transform(max);

        // transform aabbpoints and compare with min and max
        for (Simian::s32 i = 0; i < 6; ++i)
        {
            transform->World().Transform(aabbPoints[i]);
            min = chooseMin_(min, aabbPoints[i]);
            max = chooseMax_(max, aabbPoints[i]);
        }
    }

    void GCEditor::draw_( Simian::DelegateParameter& )
    {
        Simian::Game::Instance().GraphicsDevice().Draw(vertexBuffer_, Simian::GraphicsDevice::PT_LINE, width_ + height_ + 2);
    }

    void GCEditor::drawAABB_()
    {
        // draw bounding box for selected object
        if (selectedObject_)
        {
            // find the min and max all selected objects
            computeAABB_(selectedMin_, selectedMax_, selectedModel_[0], selectedTransform_[0]);
            Simian::Vector3 mid = selectedMin_ + 0.5f * (selectedMax_ - selectedMin_);
            Simian::Vector3 size = selectedMax_ - selectedMin_ + Simian::Vector3(AABB_PADDING,AABB_PADDING,AABB_PADDING);
            DebugRendererDrawCube(mid, size, Simian::Color(1.0f, 0.0f, 0.0f, 1.0f));

            for (Simian::u32 i = 1; i < selectedModel_.size(); ++i)
            {
                Simian::Vector3 currmin, currmax;
                computeAABB_(currmin, currmax, selectedModel_[i], selectedTransform_[i]);
                selectedMin_ = chooseMin_(selectedMin_, currmin);
                selectedMax_ = chooseMax_(selectedMax_, currmax);

                // draw small bounding box
                Simian::Vector3 mid = currmin + 0.5f * (currmax - currmin);
                Simian::Vector3 size = currmax - currmin + Simian::Vector3(AABB_PADDING,AABB_PADDING,AABB_PADDING);
                DebugRendererDrawCube(mid, size, Simian::Color(1.0f, 0.0f, 0.0f, 1.0f));
            }

            mid = selectedMin_ + 0.5f * (selectedMax_ - selectedMin_);
            size = selectedMax_ - selectedMin_ + Simian::Vector3(AABB_PADDING,AABB_PADDING,AABB_PADDING);
            DebugRendererDrawCube(mid, size, Simian::Color(1.0f, 1.0f, 0.0f, 1.0f));
        }
    }

    void GCEditor::updateModifiers_()
    {
        shiftModifier_ = 
            Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_LSHIFT) || 
            Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_RSHIFT);

        ctrlModifier_ = 
            Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_LCONTROL) ||
            Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_RCONTROL);

        altModifier_ = 
            Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_ALT);

        cModifier_ = 
            Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_C);
    }

    void GCEditor::updateZoom_()
    {
        if (Simian::Mouse::OnMouseWheel() > 0)
        {
            Simian::Vector3 pos = cameraNodeTransform_->Position() + Simian::Vector3(0, 0, ZOOM_AMOUNT);
            cameraNodeTransform_->Position(pos);
        }
        else if (Simian::Mouse::OnMouseWheel() < 0)
        {
            Simian::Vector3 pos = cameraNodeTransform_->Position() - Simian::Vector3(0, 0, ZOOM_AMOUNT);
            cameraNodeTransform_->Position(pos);
        }
    }

    void GCEditor::updateCameraRotation_()
    {
        Simian::Vector2 currentMouse = Simian::Mouse::GetMouseNormalizedPosition();
        if (Simian::Mouse::IsTriggered(Simian::Mouse::KEY_MBUTTON))
            oldMousePosition_ = currentMouse;
        if (Simian::Mouse::IsPressed(Simian::Mouse::KEY_MBUTTON))
        {
            Simian::Vector2 mouseDelta = currentMouse - oldMousePosition_;

            // translate that into angles
            Simian::f32 newYaw = cameraYawTransform_->Angle().Radians() + mouseDelta.X() * X_ROTATE_SPEED;
            Simian::f32 newPitch = cameraPitchTransform_->Angle().Radians() - mouseDelta.Y() * Y_ROTATE_SPEED;

            cameraYawTransform_->Rotation(cameraYawTransform_->Axis(), Simian::Radian(newYaw));
            cameraPitchTransform_->Rotation(cameraPitchTransform_->Axis(), Simian::Radian(newPitch));

            oldMousePosition_ = currentMouse;
        }
    }

    void GCEditor::computeCoordinateFrame_()
    {
        // compute forward
        forward_ = cameraYawTransform_->World().Position() - cameraNodeTransform_->World().Position();
        forward_.Y(0);
        forward_.Normalize();

        side_ = Simian::Vector3(forward_.Z(), 0, -forward_.X());
    }

    void GCEditor::computeMovement_()
    {
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_W))
            movement_.Z(1);
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_S))
            movement_.Z(-1);
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_A))
            movement_.X(-1);
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_D))
            movement_.X(1);
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_Q))
            movement_.Y(1);
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_E))
            movement_.Y(-1);
        if (movement_.LengthSquared() != 0.0f)
            movement_.Normalize();
    }

    void GCEditor::computePickingRay_()
    {
        Simian::Vector3 intersection;
        if (Simian::Plane::Z.Intersect(
            Simian::Ray::FromProjection(cameraNodeTransform_->World(), 
            camera_->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()),
            intersection))
        {
            pickedCoordinates_ = intersection;
            pickedTile_ = Simian::Vector3(
                std::floor(intersection.X()),
                intersection.Y(),
                std::floor(intersection.Z()));

            // draw debug shape at far
            DebugRendererDrawCube(pickedTile_ + Simian::Vector3(0.5f, 0.0f, 0.5f), 
                Simian::Vector3(0.9f, 0.1f, 0.9f), Simian::Color(0,0,1));
        }
    }

    //--------------------------------------------------------------------------

    void GCEditor::OnSharedLoad()
    {
        RegisterCallback(Simian::P_RENDER, Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::render_>(this));
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::update_>(this));

        VisualTile::LoadShared();
    }

    void GCEditor::OnSharedUnload()
    {
        VisualTile::UnloadShared();
    }

    void GCEditor::OnAwake()
    {
        ParentEntity()->Serializable(false);

        // set "singleton"
        instance_ = this;
        
        // set editor mode
        editorMode_ = true;

        // load the vertex buffer/material
        vertexBuffer_->Load(EditorVertex::Format, 1);
        material_ = Simian::Game::Instance().MaterialCache().Load("Materials/EditorMaterial.mat");

        // get the transforms
        Simian::Entity* entity = ParentScene()->EntityByTypeRecursive(E_CAMERA);
        entity->ComponentByType(Simian::C_TRANSFORM, transform_);
        transform_->Position(StateEditor::Instance().LastCameraPosition());

        // make all children of camera unserializable
        for (Simian::u32 i = 0; i < entity->Children().size(); ++i)
        {
            Simian::Entity* const child = entity->Children()[i];
            child->Serializable(false);
        }

        // make sure camera yaw is not serializable
        entity = entity->ChildByName("Camera Yaw");
        entity->ComponentByType(Simian::C_TRANSFORM, cameraYawTransform_);

        entity = entity->ChildByName("Camera Pitch");
        entity->ComponentByType(Simian::C_TRANSFORM, cameraPitchTransform_);

        entity = entity->ChildByName("Camera");
        entity->ComponentByType(Simian::C_TRANSFORM, cameraNodeTransform_);
        entity->ComponentByType(Simian::C_CAMERA, camera_);

        // zoom out
        cameraNodeTransform_->Position(Simian::Vector3(0, 0, -8));

        // register modesm states
        modeSM_[EM_COLLISION].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::collisionModeEnter_>(this);
        modeSM_[EM_COLLISION].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::collisionModeUpdate_>(this);
        modeSM_[EM_COLLISION].OnExit = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::collisionModeExit_>(this);

        modeSM_[EM_SCENE].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::sceneModeEnter_>(this);
        modeSM_[EM_SCENE].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::sceneModeUpdate_>(this);
        modeSM_[EM_SCENE].OnExit = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::sceneModeExit_>(this);

        modeSM_[EM_TERRAFORM].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::terraformModeEnter_>(this);
        modeSM_[EM_TERRAFORM].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::terraformModeUpdate_>(this);
        modeSM_[EM_TERRAFORM].OnExit = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::terraformModeExit_>(this);

        // setup painting sm
        setupPaintingSM_();
    }

    void GCEditor::OnInit()
    {
        DebugRendererShow();

        // try to find a map component and use its tilemap
        GCTileMap* tileMap = GCTileMap::TileMap(ParentScene());
        if (tileMap)
        {
            tileMap_ = new TileMap();
            *tileMap_ = tileMap->TileMap();

            width_ = tileMap_->Width();
            height_ = tileMap_->Height();

            terraformMap_ = new TileMap();
            terraformMap_->Create(width_, height_);

            createVisualTiles_();

            createGrid_();
        }

        // find jukebox and turn it off
        Simian::Entity* jukebox = ParentScene()->EntityByTypeRecursive(E_JUKEBOX);
        if (jukebox)
        {
            Simian::CJukebox* jukeboxCmp;
            jukebox->ComponentByType(Simian::C_JUKEBOX, jukeboxCmp);
            jukeboxCmp->Stop();
        }

        // turn off all physics
#ifdef _DESCENDED
        GCPhysicsController::EnableAllPhysics(false);
#endif
    }

    void GCEditor::OnDeinit()
    {
        vertexBuffer_->Unload();

        // unload tilemap if its there
        if (tileMap_)
            tileMap_->Unload();
        delete tileMap_;
        tileMap_ = 0;

        if (terraformMap_)
            terraformMap_->Unload();
        delete terraformMap_;
        terraformMap_ = 0;

        // unload visual tiles
        for (Simian::u32 i = 0; i < visualTiles_.size(); ++i)
        {
            for (Simian::u32 j = 0; j < visualTiles_[i].size(); ++j)
            {
                delete visualTiles_[i][j];
                delete terraformTiles_[i][j];
            }
            visualTiles_[i].clear();
            terraformTiles_[i].clear();
        }
        visualTiles_.clear();
        terraformTiles_.clear();
    }

    void GCEditor::NewMap( Simian::u32 width, Simian::u32 height )
    {
        width_ = width;
        height_ = height;

        // create a tile map
        tileMap_ = new TileMap();
        tileMap_->Create(width, height);

        // create a tile map entity
        Simian::Entity* tileMap = ParentScene()->CreateEntity(E_TILEMAP);
        GCTileMap* tileMapComponent;
        tileMap->ComponentByType(GC_TILEMAP, tileMapComponent);
        tileMapComponent->TileMap(tileMap_);
        ParentScene()->AddEntity(tileMap);

        // create terraforming map
        terraformMap_ = new TileMap();
        terraformMap_->Create(width_, height_);

        // create visual tiles
        createVisualTiles_();

        // create grid
        createGrid_();
    }

    void GCEditor::ExportCollision( const std::string& path )
    {
        tileMap_->Save(path);
    }

    void GCEditor::SelectObject( Simian::Entity* entity, bool addToSelection )
    {
        if (!entity)
        {
            selectedModel_.clear();
            selectedTransform_.clear();
            groupOffset_.clear();
            selectedObject_ = false;
            return;
        }

        Simian::CModel* model;
        Simian::CTransform* transform;

        entity->ComponentByType(Simian::C_MODEL, model);
        entity->ComponentByType(Simian::C_TRANSFORM, transform);
        
        if (!addToSelection)
        {
            selectedModel_.clear();
            selectedTransform_.clear();
            groupOffset_.clear();

            selectedObject_ = transform != 0;
        }
        else
            selectedObject_ = !selectedObject_ ? transform != 0  : true;

        selectedModel_.push_back(model);
        selectedTransform_.push_back(transform);
    }

    void GCEditor::SpawnObject( const std::string& objectId )
    {
        // convert string to id
        if (Simian::GameFactory::Instance().EntityTable().HasValue(objectId))
        {
            Simian::s32 entityId = Simian::GameFactory::Instance().EntityTable().Value(objectId);

            // don't spawn the player, spawn the player marker instead
            entityId = entityId == E_PLAYER ? E_PLAYERMARKER : entityId;

            // create entity from pool
            Simian::Entity* entity = ParentScene()->CreateEntity(entityId);

            Simian::CTransform* transform;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);

            // if the entity has a transform then set it up so its in the center of the screen
            if (transform)
            {
                // find the center of the screen
                Simian::Ray pickingRay(Simian::Ray::FromProjection(cameraNodeTransform_->World(),
                    camera_->Camera().Projection(), Simian::Vector2::Zero));

                // intersect with floor plane at y = 0
                Simian::Vector3 intersection;
                if (Simian::Plane::Z.Intersect(pickingRay, intersection))        
                    // set the transform!
                    transform->Position(intersection);
            }

            // put it on the scene
            ParentScene()->AddEntity(entity);
        }
    }

    void GCEditor::DeleteSelectedObjects()
    {
        if (!movingSelectedObject_ && selectedObject_)
        {
            // delete all selected objects
            for (Simian::u32 i = 0; i < selectedTransform_.size(); ++i)
            {
                Simian::Entity* entity = selectedTransform_[i]->ParentEntity();
                
                if (entity->ParentEntity())
                    entity->ParentEntity()->RemoveChild(entity);
                else
                    ParentScene()->RemoveEntity(entity);
            }
            selectedModel_.clear();
            selectedTransform_.clear();
            groupOffset_.clear();
            selectedObject_ = false;
        }
    }

    void GCEditor::DeleteObject( Simian::Entity* entity )
    {
        if (entity->ParentEntity())
            entity->ParentEntity()->RemoveChild(entity);
        else
            ParentScene()->RemoveEntity(entity);

        selectedModel_.clear();
        selectedTransform_.clear();
        groupOffset_.clear();
        selectedObject_ = false;
    }

    void GCEditor::ZoomBy( int zoom )
    {
        if (zoom > 0)
        {
            Simian::Vector3 pos = cameraNodeTransform_->Position() + Simian::Vector3(0, 0, ZOOM_AMOUNT);
            pos.Z(pos.Z() > MIN_ZOOM ? MIN_ZOOM : pos.Z());
            cameraNodeTransform_->Position(pos);
        }
        else if (zoom < 0)
        {
            Simian::Vector3 pos = cameraNodeTransform_->Position() - Simian::Vector3(0, 0, ZOOM_AMOUNT);
            pos.Z(pos.Z() > MIN_ZOOM ? MIN_ZOOM : pos.Z());
            cameraNodeTransform_->Position(pos);
        }
    }

    void GCEditor::CreateAnonymousGameObject( const std::string& data, Simian::Entity* parent )
    {
        Simian::DataFileIODefault dataFile;
        dataFile.Read(Simian::DataLocation(const_cast<char*>(data.c_str()), data.size()));
        Simian::Entity* newObject = ParentScene()->CreateEntity(*dataFile.Root());

        // set transform entity to center of screen

        if (parent)
            parent->AddChild(newObject);
        else
            ParentScene()->AddEntity(newObject);
    }

    bool GCEditor::GridVisibility() const
    {
        return SIsFlagSet(visibility_, VF_GRID);
    }

    void GCEditor::GridVisibility( bool visible )
    {
        SFlagSet(visibility_, VF_GRID, visible);
    }

    bool GCEditor::CollisionVisibility() const
    {
        return SIsFlagSet(visibility_, VF_COLLISION);
    }

    void GCEditor::CollisionVisibility( bool visible )
    {
        SFlagSet(visibility_, VF_COLLISION, visible);
    }

    bool GCEditor::ObjectVisibility() const
    {
        return SIsFlagSet(visibility_, VF_OBJECTS);
    }

    void GCEditor::ObjectVisibility( bool visible )
    {
        SFlagSet(visibility_, VF_OBJECTS, visible);
    }

    void GCEditor::CameraGetInfo( Simian::Vector3& position, Simian::Vector3& rotation )
    {
        position = transform_->World().Position();
        rotation = Simian::Vector3(cameraYawTransform_->Angle().Degrees(),
                                   cameraPitchTransform_->Angle().Degrees(),
                                   cameraNodeTransform_->Position().Z());
    }
}
