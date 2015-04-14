//#include <vld.h>
#include "gfxAPI.h"
#include "gfxSystemDX.h"
#include "utilAssert.h"
#include "meshManager.h"
#include "gfxLogger.h"
#include "cameraManager.h"
#include "shaderManager.h"
#include "mesh.h"
#include "lightManager.h"
#include "lightPoint.h"
#include "lightFactory.h"
#include "configGFX.h"
#include "pivotCamera.h"
#include "vertex.h"
#include "polar.h"
#include "aabbTree.h"
#include "debugShapeFactory.h"
#include "debugShapeTypes.h"
#include "debugAABBDX10.h"
#include "debugShapeBuilder.h"
#include "boundingVolumeTree.h"
#include "boundingSphereTree.h"

namespace GFX
{
    System::System()
        :   lockLightToCam_(false),
            renderBV_(false),
            currMeshesPosCount_(4)
    {
    }

    System::~System()
    {
    }

    void System::Load()
    {
        GFX_LOG("Loading Graphics...");
        API::Instance().Load(ENGINE_GFX_WIDTH,
                             ENGINE_GFX_HEIGHT,
                             ENGINE_GFX_VSYNC,
                             Util::OS::Instance().Handle(),
                             ENGINE_GFX_FULLSCREEN,
                             ENGINE_GFX_FAR,
                             ENGINE_GFX_NEAR);

        CameraManager::Instance().Load();
        MeshManager::Instance().Load();
        ShaderManager::Instance().Load();
        LightManager::Instance().Load();
        
        LightFactory::Instance().CreateInstance(LT_POINT,currLight_);
        currLight_->SpecularPower(64.f);
        currLight_->Ambient(Color(0.1f,0.1f,0.1f));
        currLight_->Diffuse(Color(0.5f,0.5f,0.5f));
        currLight_->Specular(Color::White);
        currLight_->Pos(Math::Vec3F::Zero);
        LightManager::Instance().AddLight(currLight_);

        currMesh_ = MT_BUNNY;
        currShader_ = ST_PHONG;
        Mesh* mesh = MeshManager::Instance().GetMesh(currMesh_);
        mesh->RenderSetup();

        Vertex x,y,z,o;
        x.color_ = Color::Red;
        x.pos_   = Math::Vec3F::UnitX;

        y.color_ = Color::Green;
        y.pos_   = Math::Vec3F::UnitY;

        z.color_ = Color::Blue;
        z.pos_   = Math::Vec3F::UnitZ;

        o.pos_ = Math::Vec3F::Zero;

        unsigned int** indexArray = new unsigned int*[3];
        indexArray[0] = new unsigned int[2];
        indexArray[1] = new unsigned int[2];
        indexArray[2] = new unsigned int[2];
        indexArray[0][0] = 0;
        indexArray[0][1] = 1;
        indexArray[1][0] = 2;
        indexArray[1][1] = 3;
        indexArray[2][0] = 4;
        indexArray[2][1] = 5;
        
        std::list<unsigned int>* stripSizes = new std::list<unsigned int>;
        stripSizes->push_back(2);
        stripSizes->push_back(2);
        stripSizes->push_back(2);

        Vertex* vertArray = new Vertex[6];
        vertArray[0] = o;
        vertArray[0].color_ = x.color_;
        vertArray[1] = x;
        vertArray[2] = o;
        vertArray[2].color_ = y.color_;
        vertArray[3] = y;
        vertArray[4] = o;
        vertArray[4].color_ = z.color_;
        vertArray[5] = z;

        basisMesh_ = new Mesh(MT_TOTAL,vertArray,6,indexArray,stripSizes,0,0,Mesh::RenderMethod::RM_LINELIST);
        basisMesh_->Load(false);

        if(!currMeshesPos_)
        {
            currMeshesPos_ = new Math::Vec3F[currMeshesPosCount_ * 2];
            
            Math::PolarF polar;
            polar.Radius(0.75f);
            Math::DegreeF delta(360.f / static_cast<float>(currMeshesPosCount_));

            for(unsigned int i = 0; i < currMeshesPosCount_; ++i)
            {
                Math::Vec2F pos(polar.AsCartesian());
                currMeshesPos_[i] = Math::Vec3F(pos.X(),-0.5f,pos.Y());
                polar.Theta(polar.Theta() + delta.AsRadians());
            }

            for(unsigned int i = currMeshesPosCount_; i < currMeshesPosCount_ * 2; ++i)
            {
                Math::Vec2F pos(polar.AsCartesian());
                currMeshesPos_[i] = Math::Vec3F(pos.X(),0.5f,pos.Y());
                polar.Theta(polar.Theta() + delta.AsRadians());
            }
        }

        mesh->LoadBVs(currMeshesPos_,currMeshesPosCount_ * 2);

        Phy::AABBTree::Instance().Load();
        Phy::BoundingSphereTree::Instance().Load();
        Util::BinaryTree<Phy::BoundingVolume*>* aabbTopDownMaxDepth = Phy::AABBTree::Instance().GetTopDownTreeMaxDepth();
        Util::BinaryTree<Phy::BoundingVolume*>* aabbTopDownMinVertCount = Phy::AABBTree::Instance().GetTopDownTreeMinVertCount();

        Util::BinaryTree<Phy::BoundingVolume*>* aabbBottomUp = Phy::AABBTree::Instance().GetBottomUpTree();

        Util::BinaryTree<Phy::BoundingVolume*>* boundingSphereTopDownMaxDepth = Phy::BoundingSphereTree::Instance().GetTopDownTreeMaxDepth();
        Util::BinaryTree<Phy::BoundingVolume*>* boundingSphereTopDownMinVertCount = Phy::BoundingSphereTree::Instance().GetTopDownTreeMinVertCount();

        Util::BinaryTree<Phy::BoundingVolume*>* boundingSphereBottomUp = Phy::BoundingSphereTree::Instance().GetBottomUpTree();

        if(aabbTopDownMaxDepth)
            topDownAABBTreeMaxDepth_ = DebugShapeBuilder::Instance().BuildTree(*aabbTopDownMaxDepth);

        if(aabbTopDownMinVertCount)
            topDownAABBTreeMinVertCount_ = DebugShapeBuilder::Instance().BuildTree(*aabbTopDownMinVertCount);

        if(aabbBottomUp)
            bottomUpAABBTree_ = DebugShapeBuilder::Instance().BuildTree(*aabbBottomUp);

        if(boundingSphereTopDownMaxDepth)
            topDownBoundingSphereTreeMaxDepth_ = DebugShapeBuilder::Instance().BuildTree(*boundingSphereTopDownMaxDepth);

        if(boundingSphereTopDownMinVertCount)
            topDownBoundingSphereTreeMinVertCount_ = DebugShapeBuilder::Instance().BuildTree(*boundingSphereTopDownMinVertCount);

        if(boundingSphereBottomUp)
            bottomUpBoundingSphereTree_ = DebugShapeBuilder::Instance().BuildTree(*boundingSphereBottomUp);

        currTreeToRender_ = topDownAABBTreeMaxDepth_;
        renderTreeAtDepth_ = currTreeToRender_->MaxDepth();
        GFX_LOG("Top Down AABB Tree (Max depth)");

        GFX_LOG(API::Instance().VideoCardDescription());
        GFX_LOG("Graphics Loaded");
    }

    void System::Update( float dt )
    {
        API* api = API::InstancePtr();
        CameraManager* camMgr = CameraManager::InstancePtr();

        api->BeginScene(Color::Black);

        Mesh* mesh = MeshManager::Instance().GetMesh(currMesh_);
        PivotCamera* camera = reinterpret_cast<PivotCamera*>(camMgr->GetCamera(camMgr->GetCurrentCamera()));
        Shader* shader;
        ShaderManager::Instance().GetShader(currShader_,shader);

        if(lockLightToCam_)
            currLight_->Pos(camera->Pos());
        else
            currLight_->Pos(Math::Vec3F::Zero);

        camera->Update(dt);

        
        mesh->RenderSetup();
        mesh->Render(shader,camera,currMeshesPos_,currMeshesPosCount_ * 2);

        DebugShapeBuilder::Instance().RenderTreeNodesWithDepth(renderTreeAtDepth_,
                                                               currTreeToRender_);
        
        renderBasis(camera);
        api->EndScene();
    }

    void System::Unload()
    {
        if(topDownBoundingSphereTreeMaxDepth_)
        {
            DebugShapeBuilder::Instance().DestroyTree(topDownBoundingSphereTreeMaxDepth_);
        }

        if(basisMesh_)
        {
            basisMesh_->Unload();
            delete basisMesh_;
            basisMesh_ = 0;
        }

        if(currMeshesPos_)
        {
            delete [] currMeshesPos_;
            currMeshesPos_ = 0;
        }

        Phy::AABBTree::Instance().Unload();
        LightManager::Instance().Unload();
        ShaderManager::Instance().Unload();
        MeshManager::Instance().Unload();
        CameraManager::Instance().Unload();
        API::Instance().Unload();
    }

    //--------------------------------------------------------------------------

    void System::renderBasis( Camera* cam )
    {
        Shader* shader;
        ShaderManager::Instance().GetShader(ST_DEFAULT,shader);
        Math::Vec3F origin(Math::Vec3F::Zero);

        basisMesh_->RenderSetup();
        basisMesh_->Render(shader,cam,&origin,1);
    }
}