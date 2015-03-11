/******************************************************************************/
/*!
\file		AIEffectTypes.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTTYPES_H_
#define DESCENSION_AIEFFECTTYPES_H_

namespace Descension
{
     
    enum AIEffectType
    {
        EFF_E_FLEE_HEALTH,
        EFF_E_FLEE_LOCATION,
        EFF_E_CREEP,
        EFF_E_DASH,
        EFF_E_WANDER,
        EFF_E_DISAPPEAR,
        EFF_E_SHOOTBULLET,
        EFF_E_PLAYANIM,
        EFF_E_CREATEENT,
        EFF_S_GROUNDSMASH,
        EFF_S_DASH,
        EFF_S_PROJECTILE,
        EFF_T_FREEZE,
        EFF_TOTAL
    };
}

#endif
