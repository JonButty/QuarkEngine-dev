/*************************************************************************/
/*!
\file		ComponentTypes.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
            Gavin Yeo Wei Ming, gavin.yeo, 290000111
            Jonathan Butt, j.butt, 250000211
            Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: jason.ngai\@digipen.edu, gavin.yeo\@digipen.edu,
            j.butt\@digipen.edu, bryan.yeo\@digipen.edu
            
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_COMPONENTTYPES_H_
#define SIMIAN_COMPONENTTYPES_H_

#include "Simian/EngineComponents.h"

namespace Descension
{
    using Simian::C_TOTAL;

    enum ComponentTypes
    {
        GC_PLAYERLOGIC = C_TOTAL,
        GC_DESCENSIONCAMERA,
        GC_PLAYERBULLETLOGIC,
        GC_ENEMYBULLETLOGIC,
        GC_ENEMYPROJECTILELOGIC,
        GC_DURGLEEGGCHILD,
        GC_EXPLOSIONLOGIC,
        GC_ENEMYSPAWNEXPLOSIONLOGIC,
        GC_ROCKDEBRIS,
        GC_CRYSTALSWAP,
        GC_FADESPRITEEFFECT,
        GC_ATTRIBUTES,
        GC_AIBASE,
        GC_AIGENERIC,
        GC_AIDESTRUCTIBLE,
        GC_AIEXPCRYSTAL,
        GC_AICASTER,
        GC_AIBOSSGHOST,
        GC_AIMAJEN,
        GC_AICRYSTALGUARDIAN,
        GC_AICRYSTALGUARDIANFIST,
        GC_MAJENTRIANGLE,
        GC_ENEMYSPAWNSPOT,
        GC_PHYSICSCONTROLLER,
        GC_GAMEDATA,
        GC_GESTUREINTERPRETER,
        GC_GESTUREPUZZLE,
        GC_GESTUREPUZZLETRIGGER,
        GC_MOVABLEPLATFORM,
        GC_FLOATINGPLATFORM,
        GC_TELEPORTER,
        GC_MULTITRIGGER,
        GC_CINEMATICESCAPE,
        GC_GESTURESKILLS,
        GC_UPGRADEEXCHANGE,
        GC_UI,
        GC_LEVELSELECT,
        GC_SPRITEEDITOR,
        GC_CINEMATICUI,
        GC_UIBUTTON,
        GC_UICHECKBOX,
        GC_UISLIDER,
        GC_MAINMENU,
        GC_OPTIONSMENU,
        GC_CREDITSMENU,
        GC_HOWTOPLAYMENU,
        GC_DESTRUCTIVEUI,
        GC_GAMEOVER,
        GC_CURSOR,
        GC_TILEMAP,
        GC_CHASECAMERA,
        GC_LIGHTPROJECTOR,
        GC_LIGHTNINGEFFECT,
        GC_LIGHTNINGSPELLEFFECT,
        GC_TRAPLASERBEAM,
        GC_TRAPFIRESPOT,
        GC_PARTICLEEMITTERTRIANGLE,
        GC_PARTICLEEFFECTDEACTIVATOR,
        GC_PARTICLECHAINDEACTIVATOR,
        GC_FLAMEEFFECT,
        GC_FLAMINGSNAKEEFFECT,
        GC_ASHFADEEFFECT,
        GC_BONETRAILVELOCITYCONTROLLER,
        GC_SPAWNSEQUENCE,
        GC_OBBTRIGGER,
        GC_EVENTSCRIPT,
        GC_ENEMYDROP,
        GC_COLLECTIBLE,
        GC_SPELLCOLLECTIBLE,
        GC_SPELLCOLLECTIBLESPAWNER,
        GC_AMBIENTSOUND,
        GC_MOUSEPLANEINTERSECTION,
        GC_ENTITYSPAWNER,
        GC_CINEMATICMOTION,
        GC_DESCENDINGWALL,
        GC_EDITOR,
        GC_TOTAL
    };
}

#endif
