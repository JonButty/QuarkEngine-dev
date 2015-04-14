/*************************************************************************/
/*!
\file		SpawnHeatWait.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SPAWNHEATWAIT_H_
#define DESCENSION_SPAWNHEATWAIT_H_

#include "SpawnEvent.h"

namespace Descension
{
    class SpawnHeatWait: public SpawnEvent
    {
    private:
        Simian::u32 heat_;

        static SpawnFactory::Plant<SpawnHeatWait>::Type factoryPlant_;
    public:
        Simian::u32 Heat() const;
        void Heat(Simian::u32 val);
    public:
        SpawnHeatWait();

        bool Execute( Simian::f32 dt );
    };
}

#endif
