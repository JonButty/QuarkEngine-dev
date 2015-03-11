/*************************************************************************/
/*!
\file		GCEditor.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCEDITOR_H_
#define DESCENSION_GCEDITOR_H_

#include "Simian/EntityComponent.h"
#include "Simian/VertexBuffer.h"
#include "Simian/ExplicitType.h"
#include "Simian/FiniteStateMachine.h"

#include "TileMap.h"

#include <vector>

namespace Simian
{
    class Material;
    class CTransform;
    class CCamera;
    class CModel;
    class Entity;
    class Ray;
}

namespace Descension
{
    class VisualTile;

    struct paintingCallbacks_;

    struct EditorVertex
    {
        Simian::f32 X, Y, Z;
        Simian::u32 Diffuse;

        static const Simian::VertexFormat Format;

        EditorVertex(const Simian::Vector3& position, const Simian::Color& color)
            : X(position.X()), Y(position.Y()), Z(position.Z()), Diffuse(color.ColorU32())
        {}
    };

    class GCEditor: public Simian::EntityComponent
    {
    public:
        enum EditorMode
        {
            EM_COLLISION,
            EM_SCENE,
            EM_TERRAFORM,
            EM_TOTAL
        };

        enum PaintEvent
        {
            PE_DEFAULT,
            PE_LBUTTONDOWN,
            PE_LBUTTONUP,
            PE_RBUTTONDOWN,
            PE_RBUTTONUP,
            PE_TOTAL
        };
    private:
        enum VisibilityFlag
        {
            VF_GRID,
            VF_COLLISION,
            VF_OBJECTS
        };
    private:
        // "singleton"
        static GCEditor* instance_;

        // editor mode enabled flag
        static bool editorMode_;

        // disables control
        bool disabled_;

        // mode fsm
        Simian::FiniteStateMachine modeSM_;

        // collision map
        Simian::u32 width_;
        Simian::u32 height_;

        // "lines"
        Simian::ExplicitType<Simian::VertexBuffer*> vertexBuffer_;
        Simian::Material* material_;

        // camera nodes
        Simian::CTransform* transform_;
        Simian::CTransform* cameraYawTransform_;
        Simian::CTransform* cameraPitchTransform_;
        Simian::CTransform* cameraNodeTransform_;
        Simian::CCamera* camera_;

        // camera movement
        Simian::Vector3 forward_;
        Simian::Vector3 side_;
        Simian::Vector3 movement_;
        Simian::Vector3 acceleration_;
        Simian::Vector3 velocity_;

        // picking
        Simian::Vector3 pickedTile_;
        Simian::Vector3 pickedCoordinates_;

        // testing visual tiles
        TileMap* tileMap_;
        std::vector<std::vector<VisualTile*> > visualTiles_;
        TileMap* terraformMap_;
        std::vector<std::vector<VisualTile*> > terraformTiles_;

        // "painting" in general
        Simian::Vector3 startPos_;
        bool painted_;
        Simian::FiniteStateMachine paintingSM_;

        // snapping
        Simian::Vector3 xyzSnap_;
        Simian::f32 rotationSnap_;

        // selected object
        bool selectedObject_;
        std::vector<Simian::CTransform*> selectedTransform_;
        std::vector<Simian::CModel*> selectedModel_;
        std::vector<Simian::Vector3> groupOffset_;
        bool movingSelectedObject_;
        Simian::Vector3 selectedObjectOffset_;
        Simian::Vector3 selectedMin_;
        Simian::Vector3 selectedMax_;

        // modifiers
        bool shiftModifier_;
        bool ctrlModifier_;
        bool altModifier_;
        bool cModifier_;
        bool yTranslate_;

        // rotating the selected objects
        bool rotating_;

        // visibility flags
        Simian::u8 visibility_;

        // old mouse position
        Simian::Vector2 oldMousePosition_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCEditor> factoryPlant_;
    public:
        static GCEditor& Instance();
        static bool EditorMode();
        static void EditorMode(bool mode);
        Simian::FiniteStateMachine& ModeSM();

        Simian::Vector3 XyzSnap() const;
        void XyzSnap(Simian::Vector3 val);

        Simian::f32 RotationSnap() const;
        void RotationSnap(Simian::f32 val);

        Simian::Entity* SelectedObject() const;

        bool Disabled() const;
        void Disabled(bool val);

        bool GridVisibility() const;
        void GridVisibility(bool visible);

        bool CollisionVisibility() const;
        void CollisionVisibility(bool visible);

        bool ObjectVisibility() const;
        void ObjectVisibility(bool visible);

        Simian::CTransform* CameraBaseTransform() const;
    private:
        void createVisualTiles_();
        void createGrid_();

        Simian::Vector3 chooseMin_(const Simian::Vector3& a, const Simian::Vector3& b);
        Simian::Vector3 chooseMax_(const Simian::Vector3& a, const Simian::Vector3& b);

        // sm callbacks
        void collisionModeEnter_(Simian::DelegateParameter& param);
        void collisionModeUpdate_(Simian::DelegateParameter& param);
        void collisionModeExit_(Simian::DelegateParameter& param);
        void collisionModeLButtonStart_();
        void collisionModeLButtonUpdate_();
        void collisionModeLButtonEnd_();
        void collisionModeRButtonStart_();
        void collisionModeRButtonUpdate_();
        void collisionModeRButtonEnd_();

        void sceneModeEnter_(Simian::DelegateParameter& param);
        void sceneModeUpdate_(Simian::DelegateParameter& param);
        void sceneModeExit_(Simian::DelegateParameter& param);
        void sceneModeLButtonStart_();
        void sceneModeLButtonEnd_();
        void sceneModeRButtonStart_();
        void sceneModeRButtonEnd_();
        void computeOffset_();
        bool clickedSelectedObject_();
        void moveSelectedObject_();
        void beginMoveSelectedObject_();
        void cloneSelectedObjects_();
        void computeYOffset_();
        void moveYSelectedObject_();
        void rotateSelectedObject_();

        void terraformModeEnter_(Simian::DelegateParameter& param);
        void terraformModeUpdate_(Simian::DelegateParameter& param);
        void terraformModeExit_(Simian::DelegateParameter& param);
        void terraformModeLButtonStart_();
        void terraformModeLButtonUpdate_();
        void terraformModeLButtonEnd_();
        void terraformModeRButtonStart_();
        void terraformModeRButtonUpdate_();
        void terraformModeRButtonEnd_();
        Simian::Entity* terraformCreateEntity_(const std::vector<Simian::s32> entities,
                                               Simian::u32 x, Simian::u32 y, 
                                               Simian::Entity* parent);
        Simian::u8 terraformHasTile_(Simian::u32 x, Simian::u32 y);

        void update_(Simian::DelegateParameter& param);
        void render_(Simian::DelegateParameter& param);
        void draw_(Simian::DelegateParameter& param);
        void updatePicking_(bool addToSelection = false);
        void updatePicking_(const Simian::Ray& pickingRay, 
                            const std::vector<Simian::Entity*>& entityList, 
                            Simian::CModel*& closestModel, 
                            Simian::CTransform*& closestTransform, 
                            Simian::f32& minDist);
        void computeAABB_(Simian::Vector3& min, Simian::Vector3& max, 
                          Simian::CModel* model, Simian::CTransform* transform);
        void drawAABB_();
        void updateModifiers_();
        void updateZoom_();
        void updateCameraRotation_();

        void computeCoordinateFrame_();
        void computeMovement_();
        void computePickingRay_();

        // painting sm setup
        void setupPaintingSM_();

        // default events
        void paintDefaultUpdate_(Simian::DelegateParameter& param);

        // lbutton events
        void lButtonDownEnter_(Simian::DelegateParameter& param);
        void lButtonDownUpdate_(Simian::DelegateParameter& param);
        void lButtonUpEnter_(Simian::DelegateParameter& param);
        void lButtonUpUpdate_(Simian::DelegateParameter& param);

        // rbutton events
        void rButtonDownEnter_(Simian::DelegateParameter& param);
        void rButtonDownUpdate_(Simian::DelegateParameter& param);
        void rButtonUpEnter_(Simian::DelegateParameter& param);
        void rButtonUpUpdate_(Simian::DelegateParameter& param);
    public:
        GCEditor();
        ~GCEditor();

        void OnSharedLoad();
        void OnSharedUnload();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void NewMap(Simian::u32 width, Simian::u32 height);
        void ExportCollision(const std::string& path);
        void SelectObject(Simian::Entity* entity, bool addToSelection);
        void SpawnObject(const std::string& objectId);
        void DeleteSelectedObjects();
        void DeleteObject(Simian::Entity* entity);
        void ZoomBy(int zoom);
        void CreateAnonymousGameObject(const std::string& data, Simian::Entity* parent);
        void CameraGetInfo(Simian::Vector3& position, Simian::Vector3& rotation);

        // terraforming
        void TerraformFill(const std::vector<Simian::s32>& entities, Simian::Entity* parent);
        void TerraformFringe(const std::vector<Simian::s32>& side,
                             const std::vector<Simian::s32>& corner,
                             const std::vector<Simian::s32>& fill,
                             const std::vector<Simian::s32>& knob,
                             float knobOffset,
                             Simian::Entity* parent);

        friend static void createPaintingCallbacks_();
    };

    static const Simian::f32 CAMERA_SPEED = 70.0f;
    static const Simian::f32 CAMERA_DAMP = 8.0f;
}

#endif
