/*************************************************************************/
/*!
\file		GCPhysicsController.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCPhysicsController.h"
#include "ComponentTypes.h"
#include "TileWallCollision.h"
#include "EntityCollision.h"
#include "GCTileMap.h"

#include "Simian/Game.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/DebugRenderer.h"
#include "Simian/Utility.h"
#include "Simian/Keyboard.h"

namespace Descension
{
    const Simian::u32 GCPhysicsController::COLLISION_LIST_SIZE = 16;
    const Simian::f32 GCPhysicsController::MAX_DT = 1/60.0f;
    const Simian::f32 GCPhysicsController::MAX_STEP = 5/60.0f;

    std::vector<GCPhysicsController *> GCPhysicsController::entityVec_;

    //static variables for ALL
    std::vector<GCPhysicsController *> GCPhysicsController::fullEntList_;
    bool GCPhysicsController::enableAll_ = true;

    //GestureInterpreter GCPhysicsController::gesture;

    Simian::FactoryPlant<Simian::EntityComponent, GCPhysicsController>
        GCPhysicsController::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_PHYSICSCONTROLLER);

    GCPhysicsController::GCPhysicsController()
        : 
          bitFlags_(0),
          collisionLayer_(0),
          enabled_(false),
          collided_(false),
          transform_(NULL),
          radius_(0.2f),
          dt_(0.0f),
          dtAcc_(0.0f),
          velocity_(0.0f, 0.0f, 0.0f),
          position_(0.0f, 0.0f, 0.0f),
          tileMapPtr_(0)
    {
    }

    //--------------------------------------------------------------------------
    const Simian::CTransform* GCPhysicsController::Transform() const
    {
        return transform_;
    }

    void GCPhysicsController::EnableAllPhysics(bool flag)
    {
        enableAll_ = flag;
        if (flag) //if true, update all position
        {
            for (Simian::u32 i=0; i<fullEntList_.size(); ++i)
            {
                fullEntList_[i]->position_ =
                    fullEntList_[i]->transform_->World().Position();
            }
        }
    }

    bool GCPhysicsController::Enabled() const
    {
        return enabled_;
    }

    void GCPhysicsController::Enabled( bool val )
    {
        if (val && !enabled_)
        {   //->World().Position()
            position_ = transform_->Position(); //re-enabled check
            collided_ = false;

            //std::cout << "Added: " << entityVec_.size() << std::endl;
            entityVec_.push_back(this);
        }
        
        if (!val && enabled_)
        {
            std::vector<GCPhysicsController*>::iterator ite;
            ite = std::find(entityVec_.begin(), entityVec_.end(), this);
            if (ite != entityVec_.end())
            {
                entityVec_.erase(ite);
                collisionList_.clear();
                //std::cout << "Removed: " << entityVec_.size() << std::endl;
            }
        }
        
        enabled_ = val;
    }

    //collision behaviours
    bool GCPhysicsController::GetBitFlag(Simian::u32 bitsPlace) const
    {
        return SIsFlagSet(bitFlags_, bitsPlace);
    }

    void GCPhysicsController::SetBitFlags(Simian::u32 bitsPlace)
    {
        SFlagSet(bitFlags_, bitsPlace);
    }

    void GCPhysicsController::UnsetBitFlags(Simian::u32 bitsPlace)
    {
        SFlagUnset(bitFlags_, bitsPlace);
    }

    Simian::u8 GCPhysicsController::CollisionLayer(void) const
    {
        return collisionLayer_;
    }

    void GCPhysicsController::CollisionLayer(Simian::u8 val)
    {
        collisionLayer_ = val;
    }

    Simian::f32 GCPhysicsController::Radius() const
    {
        return radius_;
    }

    void GCPhysicsController::Radius( Simian::f32 val )
    {
        radius_ = val;
    }

    const Simian::Vector3& GCPhysicsController::Velocity() const
    {
        return velocity_;
    }

    void GCPhysicsController::Velocity( const Simian::Vector3 &val )
    {
        velocity_ = val;
    }

    const Simian::Vector3& GCPhysicsController::Position() const
    {
        return position_;
    }

    void GCPhysicsController::Position(const Simian::Vector3& newpos)
    {
        position_ = newpos;
    }

    bool GCPhysicsController::Collided() const
    {
        return collided_;
    }

    GCTileMap* GCPhysicsController::TileMapPtr() const
    {
        return tileMapPtr_;
    }
    void GCPhysicsController::TileMapPtr(GCTileMap* val)
    {
        tileMapPtr_ = val;
    }

    std::vector<GCPhysicsController *>* GCPhysicsController::GetCollisionList()
    {
        return &collisionList_;
    }

    //--------------------------------------------------------------------------
    void GCPhysicsController::reset_(Simian::DelegateParameter&)
    {
        //reset data
        collisionList_.clear();
        collided_ = false;
    }

    void GCPhysicsController::update_( Simian::DelegateParameter&)
    {
        if (!enabled_ || !enableAll_) //disabled anyway
            return;

        dtAcc_ += Simian::Game::Instance().FrameTime();
        dtAcc_ = Simian::Math::Min(MAX_STEP, dtAcc_);
        //dt_ += param.As<Simian::EntityUpdateParameter>().Dt;
        
        //debug drawing
        DebugRendererDrawCircle(Simian::Vector3(position_.X()
            , position_.Y()+radius_,
            position_.Z()),
            radius_,
            Simian::Color(0.0f, 1.0f, 0.0f));
        
        //DT: step down for lag as precaution
        while (dtAcc_ >= MAX_DT)
        {
            dt_ = Simian::Math::Min(
                MAX_DT*Simian::Game::Instance().TimeMultiplier(), MAX_DT);
            physicsUpdate_();
            dtAcc_ -= MAX_DT;
        }
    }

    void GCPhysicsController::physicsUpdate_(void)
    {
        //----------------------------------------------------------------------
        // SCALE UP, REDUCE ERROR
        //----------------------------------------------------------------------
        //debug core radius
        position_ *= TileWallCollision::PHYSICS_SCALE;
        Simian::f32 dt = Simian::Math::Min(dt_,
                         MAX_DT*Simian::Game::Instance().TimeMultiplier());

        //----------------------------------------------------------------------
        // ENTITY / PROPS
        //----------------------------------------------------------------------
        //if flag1 & flag2 != 0, collide check
        Simian::Vector3 posTemp(position_);
        Simian::Vector3 velTemp(velocity_ * dt * TileWallCollision::PHYSICS_SCALE);
        Simian::Vector3 velAB(velTemp);
        for (Simian::u32 i=0; i<entityVec_.size(); ++i)
        {
            //don't check against self
            if (entityVec_[i] == this)
            {
                continue;
            }
            //not this and same layer
            if ( ((entityVec_[i]->collisionLayer_ & collisionLayer_) > 1 &&
                !entityVec_[i]->GetBitFlag(PHY_PLAYER) &&
                entityVec_[i]->GetBitFlag(PHY_ENEMY) != GetBitFlag(PHY_ENEMY)) ||
                //props
                (GetBitFlag(PHY_GENERAL)) ||
                (!GetBitFlag(PHY_PROP) && entityVec_[i]->GetBitFlag(PHY_PROP)) ||
                (GetBitFlag(PHY_PROJ_HOSTILE) && entityVec_[i]->GetBitFlag(PHY_PLAYER)))
            {
                //projectile
                if (!(GetBitFlag(PHY_ENEMY) && entityVec_[i]->GetBitFlag(PHY_PROJ_FRIENDLY)))
                {
                    if (EntityCollision::CheckEntityCollision(posTemp,
                        velAB,
                        entityVec_[i]->position_ * TileWallCollision::PHYSICS_SCALE,
                        entityVec_[i]->velocity_ * dt * TileWallCollision::PHYSICS_SCALE,
                        radius_ * TileWallCollision::PHYSICS_SCALE,
                        entityVec_[i]->radius_ * TileWallCollision::PHYSICS_SCALE,
                        dt,
                        true))
                    {
                        //add to list if not found
                        std::vector<GCPhysicsController*>::iterator ite;
                        ite = std::find(collisionList_.begin(), collisionList_.end(),
                            entityVec_[i]);
                        if (ite == collisionList_.end())
                        {
                            entityVec_[i]->collisionList_.push_back(this);
                            this->collisionList_.push_back(entityVec_[i]);
                        }
                        collided_ = true;
                        entityVec_[i]->collided_ = true;
                        //remove Y
                        velTemp = velAB;
                        velTemp.Y(0.0f);
                    }
                }
            }
        }
        
        //----------------------------------------------------------------------
        // TILE
        //----------------------------------------------------------------------
        // WHERE layer0 = tile & walls
        if (SIsFlagSet(collisionLayer_, Collision::COLLISION_L0))
        {
            if (TileWallCollision::CheckTileCollision(position_,
                velTemp,
                radius_ * TileWallCollision::PHYSICS_SCALE,
                &(tileMapPtr_->TileMap()), true, GetBitFlag(PHY_COLLIDE_VOID)))
            {
                collided_ = true;
            }
        }
        else
        {
            //collision layer0 is off, update as usual
            velocity_.Y(0.0f);
            position_ += velocity_ * dt * TileWallCollision::PHYSICS_SCALE;
        }

        //----------------------------------------------------------------------
        // TRANSFORM BACK TO NORMAL CO-ORDINATES
        //----------------------------------------------------------------------
        position_ /= TileWallCollision::PHYSICS_SCALE;
        // TODO: set transform position based on above
        transform_->Position(
            (transform_->ParentTransform() ? transform_->
            ParentTransform()->World().Inversed() : Simian::Matrix::Identity)*
            position_);
    }

    //--------------------------------------------------------------------------

    void GCPhysicsController::OnSharedLoad()
    {
        RegisterCallback(Simian::P_PHYSICS, Simian::Delegate::
            CreateDelegate<GCPhysicsController, &GCPhysicsController::update_>(this));
        RegisterCallback(Simian::P_PHYSICS_RESET, Simian::Delegate::
            CreateDelegate<GCPhysicsController, &GCPhysicsController::reset_>(this));
    }

    void GCPhysicsController::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        position_ = transform_->World().Position();//global
        SetCollisionLayer(Collision::COLLISION_L0);
        collisionList_.reserve(COLLISION_LIST_SIZE);
        if (enabled_)
        {
            enabled_ = false;
            Enabled(true); //push correctly
        }

        fullEntList_.push_back(this);
    }

    void GCPhysicsController::OnInit()
    {
        tileMapPtr_ = GCTileMap::TileMap( ParentScene() );
    }

    void GCPhysicsController::OnDeinit()
    {
        Enabled(false);

        std::vector<GCPhysicsController*>::iterator ite;
        ite = std::find(fullEntList_.begin(), fullEntList_.end(), this);
        if (ite != fullEntList_.end())
        {
            fullEntList_.erase(ite);
        }
    }

    void GCPhysicsController::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Enabled", enabled_);
        data.AddData("Radius", radius_);
        
        //set behaviour
        if (!GetBitFlag(PHY_PROP))
        {
            if (GetBitFlag(PHY_PLAYER))
            {
                data.AddData("Behaviour", "Player");
                return;
            }
            if (GetBitFlag(PHY_ENEMY))
            {
                data.AddData("Behaviour", "Enemy");
                return;
            }
            if (GetBitFlag(PHY_PROJ_FRIENDLY))
            {
                data.AddData("Behaviour", "Bullet_Friendly");
                return;
            }
            if (GetBitFlag(PHY_PROJ_HOSTILE))
            {
                data.AddData("Behaviour", "Bullet_Hostile");
                return;
            }
        }
    }

    void GCPhysicsController::Deserialize( const Simian::FileObjectSubNode& data )
    {
        const Simian::FileDataSubNode* enabledNode = data.Data("Enabled");
        enabled_ = enabledNode ? enabledNode->AsBool() : true;

        const Simian::FileDataSubNode* radiusNode = data.Data("Radius");
        radius_ = radiusNode ? radiusNode->AsF32() : radius_;

        //get type and set layers
        const Simian::FileDataSubNode* behaviourNode = data.Data("Behaviour");
        std::string type = behaviourNode ? behaviourNode->AsString() : "";
        //std::cout << "type=" << type << std::endl;
        if (type == "")
        {
            SetBitFlags(PHY_PROP);
            //SetBitFlags(PHY_COLLIDE_VOID);
        }
        if (type == "Player")
        {
            SetBitFlags(PHY_PLAYER);
            SetBitFlags(PHY_COLLIDE_VOID);
            SetCollisionLayer(Collision::COLLISION_L1);
            SetCollisionLayer(Collision::COLLISION_L3);
            return;
        }
        if (type == "Enemy")
        {
            SetBitFlags(PHY_ENEMY);
            SetBitFlags(PHY_COLLIDE_VOID);
            SetCollisionLayer(Collision::COLLISION_L1);
            SetCollisionLayer(Collision::COLLISION_L2);
            return;
        }
        if (type == "Bullet_Friendly")
        {
            SetBitFlags(PHY_PROJ_FRIENDLY);
            UnsetBitFlags(PHY_COLLIDE_VOID);
            SetCollisionLayer(Collision::COLLISION_L2);
            return;
        }
        if (type == "Bullet_Hostile")
        {
            SetBitFlags(PHY_PROJ_HOSTILE);
            UnsetBitFlags(PHY_COLLIDE_VOID);
            SetCollisionLayer(Collision::COLLISION_L3);
            return;
        }
        if (type == "General")
        {
            SetBitFlags(PHY_GENERAL);
            SetBitFlags(PHY_COLLIDE_VOID);
            SetCollisionLayer(Collision::COLLISION_L1);
            SetCollisionLayer(Collision::COLLISION_L2);
            return;
        }
    }

    //--------------------------------------------------------------------------
    void GCPhysicsController::SetCollisionLayer(Simian::u32 bitPlacement)
    {
        SFlagSet(collisionLayer_, bitPlacement);
    }

    void GCPhysicsController::UnsetCollisionLayer(Simian::u32 bitPlacement)
    {
        SFlagUnset(collisionLayer_, bitPlacement);
    }
}
