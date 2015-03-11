/******************************************************************************/
/*!
\file		AIEffectFlee.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Scene.h"
#include "Simian/CTransform.h"
#include "Simian/Interpolation.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/EngineComponents.h"
#include "EntityTypes.h"

#include "ComponentTypes.h"
#include "AIEffectFlee.h"
#include "AIEffectTypes.h"

#include "GCAttributes.h"
#include "GCPhysicsController.h"
#include "Pathfinder.h"
#include "TileMap.h"

namespace Descension
{
	Simian::FactoryPlant<AIEffectBase, AIEffectFlee> AIEffectFlee::factoryPlant_(
		AIEffectBase::createFactoryPlant_<AIEffectFlee>(EFF_E_FLEE));

	// Movement Properties
	const Simian::f32 HALF_TILE = 0.5f;
	const Simian::f32 RAND_AREA_TILE = 0.1f;
	const Simian::f32 MAX_STEER_TIME = 0.7f;

	AIEffectFlee::AIEffectFlee()
		:	AIEffectBase(EFF_E_FLEE),
			map_(0),
			sourceTransform_(0),
			targetTransform_(0),
			model_(0),
			victim_(false),
			healthMin_(0),
			steerTimer_(0.0f),
			dirChangeTimer_(0.0f),
			dirChangeIntervalMin_(0.0f),
			dirChangeIntervalMax_(2.0f),
			fleeTimer_(0.0f),
			fleeTimeMin_(0.0f),
			fleeTimeMax_(2.0f),
			nextTile_(0)
	{
		dirSequence_.reserve(DIR_TOTAL);
	}

	AIEffectFlee::~AIEffectFlee()
	{
	}

	//-Private------------------------------------------------------------------
	void AIEffectFlee::onAwake_()
	{

	}

	void AIEffectFlee::onInit_(GCAttributes* )
	{
		Target()->ParentScene()->EntityByType(E_TILEMAP)->ComponentByType(GC_TILEMAP,map_);
		Target()->ComponentByType(Simian::C_TRANSFORM, targetTransform_);
		Target()->ComponentByType(Descension::GC_PHYSICSCONTROLLER, targetPhysics_);
		Target()->ComponentByType(Simian::C_MODEL,model_);

		if(!map_ || !targetTransform_ || !targetPhysics_ || !model_)
			Enable(false);
	}

	void AIEffectFlee::onUnload_()
	{
	}

	void AIEffectFlee::loadVictim_()
	{
		if(Target()->Victim() && Target()->Victim()->ParentEntity())
		{
			Target()->Victim()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,sourceTransform_);
			victim_ = true;
			return;
		}
		victim_ = false;
	}

	//-Public-------------------------------------------------------------------

	bool AIEffectFlee::Update(Simian::f32 dt )
	{
		if(!Enable()) 
			return false;
		
		if(!victim_)
			loadVictim_();

		if(victim_ && Target()->Health() > 0 && Target()->Health() < healthMin_ && fleeTimer_ < fleeTimeMax_)
		{
			// Enters here for the first time
			if(OnEnter())
			{   
				OnEnter(false);
                fleeTimeMax_ = Simian::Math::Random(fleeTimeMin_,fleeTimeMax_);
				Simian::Vector3 p1(targetTransform_->World().Position());
				Simian::Vector3 p0(sourceTransform_->World().Position());
				Simian::Vector3 dir =  p1 - p0;
				if(dir.LengthSquared() > Simian::Math::EPSILON)
					dir.Normalize();
				fleeDir_ = convertToDirection(p0,p1);
				currentDir_ = convertToDirection(p1,p0);

				model_->Controller().PlayAnimation("Run",true);

				updateNextTile_(dt);
			}
			if(reachedTile_(dt))
				updateNextTile_(dt); 

			steer_(dt);
			fleeTimer_ += dt;

			return true;
		}
		
		if(!OnEnter())
		{
			OnEnter(true);
			if(fleeTimer_ >= fleeTimeMax_)
				model_->Controller().PlayAnimation("Run");\

		}
		return false;
	}

	void AIEffectFlee::serialize_( Simian::FileObjectSubNode& data)
	{
		data.AddData("HealthMin",healthMin_);
		data.AddData("HealthMin",healthMin_);
		data.AddData("FleeTimeMin",fleeTimeMin_);
		data.AddData("FleeTimeMax",fleeTimeMax_);
		data.AddData("DirChangeIntervalMax",dirChangeIntervalMax_);
		data.AddData("DirChangeIntervalMin",dirChangeIntervalMin_);
	}

	void AIEffectFlee::deserialize_( const Simian::FileObjectSubNode& data)
	{
		const Simian::FileDataSubNode* temp = data.Data("HealthMin");
		healthMin_ = temp? temp->AsF32() : healthMin_;

		temp = data.Data("FleeTimeMin");
		fleeTimeMin_ = temp? temp->AsF32() : fleeTimeMin_;
		temp = data.Data("FleeTimeMax");
		fleeTimeMax_ = temp? temp->AsF32() : fleeTimeMax_;
		
		temp = data.Data("DirChangeIntervalMax");
		dirChangeIntervalMax_ = temp? temp->AsF32() : dirChangeIntervalMax_;
		temp = data.Data("DirChangeIntervalMin");
		dirChangeIntervalMin_ = temp? temp->AsF32() : dirChangeIntervalMin_;

		dirChangeTimer_ = dirChangeIntervalMax_;
	}

	//-Private------------------------------------------------------------------

	bool AIEffectFlee::reachedTile_( Simian::f32 dt )
	{
		Simian::f32 futureDist = Target()->MovementSpeed() * dt;
		futureDist *= futureDist;
		Simian::Vector3 currentDist(nextTile_ - targetTransform_->World().Position());
		return currentDist.LengthSquared() > futureDist;
	}

	void AIEffectFlee::updateNextTile_( Simian::f32 dt )
	{
		loadDirSequence_();
		steerTimer_ = 0.0f;
		dirChangeTimer_ += dt;

		Simian::Vector3 targetPos(targetTransform_->World().Position());

		Simian::Vector2 pos(targetPos.X(),targetPos.Z());
		pos = PathFinder::Instance().WorldToSubtile(pos);
		determineDir_(pos);

		pos = PathFinder::Instance().SubtileToWorld(pos);
		nextTile_.X(pos.X());
		nextTile_.Z(pos.Y());
		Simian::Vector3 dir = nextTile_ - targetPos;

		if(dir.LengthSquared() < Simian::Math::EPSILON)
			finalDir_ = Simian::Vector3::Zero;
		else
		{
			Target()->MovementSpeed(Target()->MovementSpeedMax());
			finalDir_ = dir.Normalized();
		}

		startDir_ = Target()->Direction();
	}

	void AIEffectFlee::determineDir_( Simian::Vector2& subTilePos )
	{
		Simian::u32 minX = 0, minY = 0, maxX = 0, maxY = 0;
		PathFinder::Instance().CalcMinMax(targetPhysics_->Radius(),
			static_cast<Simian::u32>(subTilePos.X()),static_cast<Simian::u32>(subTilePos.Y()),minX,minY,maxX,maxY);

		PathFinder::Instance().SetNewPosition(targetTransform_->World().Position(),targetPhysics_->Radius(),oldMin_,oldMax_);

		Simian::u32 maxNumDir = dirSequence_.size() - 1;
		Simian::u32 i = 0;

		if(dirChangeTimer_ >= dirChangeIntervalMax_)
			dirChangeTimer_ = Simian::Math::Random(dirChangeIntervalMin_,dirChangeIntervalMax_);
		
		std::vector<Simian::u32>::iterator it = dirSequence_.begin();
		bool validDir = false;

		// Determine direction
		while( dirSequence_.size() > 1 )
		{
			if(*it == DIR_NORTH)
			{
				Simian::u32 y = maxY + 1;
				if( targetPhysics_->Collided() || !checkSpan_(y,minX, maxX, Tile::TILE_NORTH) || (*it == fleeDir_))
				{
					dirSequence_.erase(it);
					i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
					it = dirSequence_.begin() + i;
					continue;
				}
				else
				{
					subTilePos.Y(subTilePos.Y()+1);
					currentDir_ = DIR_NORTH;
					validDir = true;
					break;
				}                
			}
			if(*it == DIR_EAST)
			{
				Simian::u32 x = maxX + 1;
				if( targetPhysics_->Collided() || !checkSpan_(x,minY, maxY, Tile::TILE_EAST) || (*it == fleeDir_))
				{
					dirSequence_.erase(it);
					i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
					it = dirSequence_.begin() + i;
					continue;
				}
				else
				{
					subTilePos.X(subTilePos.X()+1);
					currentDir_ = DIR_EAST;
					validDir = true;
					break;
				}
			}
			if(*it == DIR_SOUTH )
			{
				Simian::u32 y = minY - 1;
				if( targetPhysics_->Collided() || !checkSpan_(y,minX, maxX, Tile::TILE_SOUTH) || (*it == fleeDir_))
				{
					dirSequence_.erase(it);
					i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
					it = dirSequence_.begin() + i;
					continue;
				}
				else
				{
					subTilePos.Y(subTilePos.Y()-1);
					currentDir_ = DIR_SOUTH;
					validDir = true;
					break;
				}
			}
			if(*it == DIR_WEST)
			{
				Simian::u32 x = minX - 1;
				if( targetPhysics_->Collided() || !checkSpan_(x,minY, maxY, Tile::TILE_WEST) || (*it == fleeDir_))
				{
					dirSequence_.erase(it);
					i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
					it = dirSequence_.begin() + i;
					continue;
				}
				else
				{
					subTilePos.X(subTilePos.X()-1);
					currentDir_ = DIR_WEST;
					validDir = true;
					break;
				}
			}
		}
		if(!validDir)
		{
			if(currentDir_ == DIR_NORTH)
				subTilePos.Y(subTilePos.Y()+1);
			else if(currentDir_ == DIR_SOUTH)
				subTilePos.Y(subTilePos.Y()-1);
			else if(currentDir_ == DIR_EAST)
				subTilePos.X(subTilePos.X()+1);
			else
				subTilePos.X(subTilePos.X()-1);
		}
	}

	void AIEffectFlee::steer_( Simian::f32 dt )
	{
		steerTimer_ += dt;

		if(steerTimer_<= MAX_STEER_TIME)
		{
			Simian::f32 intp = steerTimer_/MAX_STEER_TIME;
			intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
			Target()->Direction(Simian::Interpolation::Interpolate<Simian::Vector3>(startDir_,finalDir_,intp,Simian::Interpolation::EaseCubicOut));
		}
		Simian::f32 angle = vectorToDegreeFloat_(Target()->Direction());
		targetTransform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
	}

	Simian::f32 AIEffectFlee::vectorToDegreeFloat_(const Simian::Vector3& vec)
	{
		// Get angle of rotation in degrees for vector
		//float x = Simian::Math::Clamp(vec.X(), -1.0f, 1.0f);
		if(vec.LengthSquared() > Simian::Math::EPSILON)
		{
			Simian::Vector3 tempVec = vec.Normalized();
			Simian::f32 angle = std::acosf(tempVec.X());
			if (tempVec.Z() > 0)
				angle = Simian::Math::TAU - angle;
			// Wrap angle within 360
			angle = Simian::Math::WrapAngle(Simian::Radian(angle)).Degrees();

			return angle;
		}
		return 0;
	}

	Simian::u32 AIEffectFlee::convertToDirection( const Simian::Vector3& p1, const Simian::Vector3& p0 )
	{
		Simian::Vector3 dir = p1 - p0;
		if(dir.LengthSquared() < Simian::Math::EPSILON)
			return DIR_ERR;
		dir.Normalize();

		Simian::f32 angle = vectorToDegreeFloat_(dir);

		if(((angle >= 0.0f) && (angle < 45.0f)) || ((angle > 315.0f) && (angle < 360.0f)))
			return DIR_EAST;
		else if((angle >= 45.0f) && (angle <= 135.0f))
			return DIR_SOUTH;
		else if((angle > 45.0f) && (angle < 225.0f))
			return DIR_WEST;
		else
			return DIR_NORTH;
	}

	void AIEffectFlee::loadDirSequence_()
	{
		dirSequence_.clear();
		dirSequence_.push_back(currentDir_);
		
		Simian::u32 i = 0; 
		while( i < DIR_TOTAL )
		{
			if( i == DIR_NORTH )
			{
				if( (fleeDir_ != DIR_NORTH) && (currentDir_ != DIR_NORTH) )
					dirSequence_.push_back(DIR_NORTH);
				++i;
				continue;
			}
			if( i == DIR_EAST )
			{
				if( (fleeDir_ != DIR_EAST) && (currentDir_ != DIR_EAST) )
					dirSequence_.push_back(DIR_EAST);
				++i;
				continue;
			}
			if( i == DIR_SOUTH )
			{
				if( (fleeDir_ != DIR_SOUTH) && (currentDir_ != DIR_SOUTH) )
					dirSequence_.push_back(DIR_SOUTH);
				++i;
				continue;
			}
			if( i == DIR_WEST )
			{
				if( (fleeDir_ != DIR_WEST) && (currentDir_ != DIR_WEST) )
					dirSequence_.push_back(DIR_WEST);
				++i;
				continue;
			}
		}
		dirSequence_.push_back(fleeDir_);
	}

	bool AIEffectFlee::checkSpan_( Simian::u32 constAxis, Simian::u32 min, Simian::u32 max, Simian::u32 dir )
	{
		// Vertical
		if( (dir == Tile::TILE_NORTH) || (dir == Tile::TILE_SOUTH) )
		{
			while(min <= max)
			{
				if( !PathFinder::Instance().IsNextSubTileValid(dir,min,constAxis) )
					return false;
				++min;
			}
			return true;
		}
		// Horizontal
		else
		{
			while(min <= max)
			{
				if( !PathFinder::Instance().IsNextSubTileValid(dir,constAxis,min) )
					return false;
				++min;
			}
			return true;
		}            
	}
}
