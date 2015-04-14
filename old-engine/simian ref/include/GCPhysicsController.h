/*************************************************************************/
/*!
\file		GCPhysicsController.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCPHYSICSCONTROLLER_H_
#define DESCENSION_GCPHYSICSCONTROLLER_H_

#include "TileMap.h"
#include "GCTileMap.h"
#include "GCGestureInterpreter.h"

#include "Simian/CTransform.h"
#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include <vector>

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCPhysicsController : public Simian::EntityComponent
    {
    public:
        static const Simian::u32 COLLISION_LIST_SIZE;
        static const Simian::f32 MAX_DT, MAX_STEP;

        //bitflags
        enum PHYSICS_BITFLAGS
        {
            PHY_COLLIDE_VOID = 0,
            PHY_PLAYER,
            PHY_ENEMY,
            PHY_PROP,
            PHY_PROJ_FRIENDLY,
            PHY_PROJ_HOSTILE,
            PHY_GENERAL, //--CollideALL

            PHY_TOTAL_BITS
        };

    private:
        Simian::u8 bitFlags_;
        Simian::u8 collisionLayer_;
        bool enabled_, collided_;
        Simian::CTransform* transform_;
        Simian::f32 radius_, dt_, dtAcc_;

        Simian::Vector3 velocity_;
        Simian::Vector3 position_; //no get, no set
        GCTileMap *tileMapPtr_;

        std::vector<GCPhysicsController *> collisionList_;

        //list of entity transform
        static std::vector<GCPhysicsController *> entityVec_;
        
        //static variable for ALL
        static std::vector<GCPhysicsController *> fullEntList_;
        static bool enableAll_;

        static Simian::FactoryPlant<Simian::EntityComponent,
                                    GCPhysicsController>
                                    factoryPlant_;
    public:
        const Simian::CTransform* Transform() const;

        static void EnableAllPhysics(bool flag);

        bool Enabled() const;
        void Enabled(bool val);

        //collision behaviours
        bool GetBitFlag(Simian::u32 bitsPlace) const;
        void SetBitFlags(Simian::u32 bitsPlace);
        void UnsetBitFlags(Simian::u32 bitsPlace);

        //returns reference to collisionLayer_ bits set: editable
        Simian::u8 CollisionLayer(void) const;
        void CollisionLayer(Simian::u8 val);

        Simian::f32 Radius() const;
        void Radius(Simian::f32 val);

        const Simian::Vector3& Velocity() const;
        void Velocity( const Simian::Vector3 &val );

        const Simian::Vector3& Position() const;
        void Position(const Simian::Vector3& newpos);

        bool Collided() const;

        Simian::f32 DT() const;

        GCTileMap* TileMapPtr() const;
        void TileMapPtr(GCTileMap* val);

        std::vector<GCPhysicsController *>* GetCollisionList();

    private:
        void reset_(Simian::DelegateParameter& param);
        void update_(Simian::DelegateParameter& param);
        //to be run iteratively when DT exceeds
        void GCPhysicsController::physicsUpdate_(void);

    public:
        GCPhysicsController();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);

        void SetCollisionLayer(Simian::u32 bitPlacement);
        void UnsetCollisionLayer(Simian::u32 bitPlacement);
    };
}

#endif
