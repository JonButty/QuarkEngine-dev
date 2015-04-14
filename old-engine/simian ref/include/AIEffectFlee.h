/******************************************************************************/
/*!
\file		AIEffectFlee.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTFLEE_H_
#define DESCENSION_AIEFFECTFLEE_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/Vector3.h"

#include "AIEffectBase.h"

namespace Simian
{
	class CTransform;
    class CModel;
}

namespace Descension
{
	class GCAttributes;
	class GCTileMap;
    class GCPhysicsController;

	class AIEffectFlee : public AIEffectBase
	{
        enum DIRECTION
        {
            DIR_NORTH,
            DIR_EAST,
            DIR_SOUTH,
            DIR_WEST,
            DIR_TOTAL,
            DIR_ERR
        };

	private:
		GCTileMap* map_;
		Simian::CTransform* sourceTransform_;
		Simian::CTransform* targetTransform_;
        GCPhysicsController * targetPhysics_;
        Simian::CModel* model_;
		bool victim_;
        Simian::u32 fleeDir_, currentDir_;
        Simian::f32 healthMin_;
        Simian::f32 steerTimer_;
        Simian::f32 dirChangeTimer_;
        Simian::f32 dirChangeIntervalMin_,dirChangeIntervalMax_;
        Simian::f32 fleeTimer_;
        Simian::f32 fleeTimeMin_, fleeTimeMax_;
        Simian::Vector2 oldMin_, oldMax_;
		Simian::Vector3 startDir_;
		Simian::Vector3 finalDir_;
		Simian::Vector3 nextTile_;
        std::vector<Simian::u32> dirSequence_;
		
		static Simian::FactoryPlant<AIEffectBase, AIEffectFlee> factoryPlant_;
        
    private:
        void onAwake_();
		void onInit_( GCAttributes* source );
		void onUnload_();
		void loadVictim_();
        bool reachedTile_(Simian::f32 dt);
        void updateNextTile_(Simian::f32 dt);
        void determineDir_(Simian::Vector2& subTilePos);
		void steer_( Simian::f32 dt );
		Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        Simian::u32 convertToDirection(const Simian::Vector3& p1, const Simian::Vector3& p0);
        void loadDirSequence_();
        bool checkSpan_(Simian::u32 constAxis, Simian::u32 min, Simian::u32 max, Simian::u32 dir);
        void serialize_( Simian::FileObjectSubNode& node);
        void deserialize_( const Simian::FileObjectSubNode& node);
	public:
		AIEffectFlee();
        ~AIEffectFlee();
		bool Update( Simian::f32 dt);
	};
}

#endif
