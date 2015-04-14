/************************************************************************/
/*!
\file		CBoneTrail.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CBONETRAIL_H_
#define SIMIAN_CBONETRAIL_H_

#include "SimianPlatform.h"
#include "PointTrail.h"
#include "VertexBuffer.h"
#include "EntityComponent.h"

namespace Simian
{
    class CModel;
    class Material;
    class GraphicsDevice;
    class MaterialCache;
    class AnimationJoint;
    class RenderQueue;

    class SIMIAN_EXPORT CBoneTrail: public EntityComponent
    {
    private:
        std::string firstJoint_;
        std::string secondJoint_;
        std::string target_;
        std::string materialFile_;
        u32 segmentCount_;
        f32 trailDuration_;
        f32 opacity_;

        PointTrail trail_;
        CModel* model_;
        AnimationJoint* joints_[2];
        VertexBuffer* vertexBuffer_;
        Material* material_;
        std::vector<PositionColorUVVertex> vertices_;

        static GraphicsDevice* device_;
        static MaterialCache* materialCache_;
        static RenderQueue* renderQueue_;

        static FactoryPlant<EntityComponent, CBoneTrail> factoryPlant_;
    public:
        static GraphicsDevice* Device();
        static void Device(GraphicsDevice* val);

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        static Simian::RenderQueue* RenderQueue();
        static void RenderQueue(Simian::RenderQueue* val);

        Simian::f32 Opacity() const;
        void Opacity(Simian::f32 val);

        AnimationJoint* TopJoint() const;
        AnimationJoint* BottomJoint() const;

        Vector3 TopJointPosition() const;
        Vector3 BottomJointPosition() const;
    private:
        Vector3 jointPosition_(AnimationJoint* joint) const;
        void updateVertexBuffer_();
        void drawTrail_(Simian::DelegateParameter& p);

        void update_(Simian::DelegateParameter& p);
        void render_(Simian::DelegateParameter&);
    public:
        CBoneTrail();
        ~CBoneTrail();

        void OnSharedLoad();
        void OnInit();
        void OnDeinit();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );

    };
}

#endif
