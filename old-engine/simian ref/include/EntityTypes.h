/*************************************************************************/
/*!
\file		EntityTypes.h
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
#ifndef DESCENSION_ENTITYTYPES_H_
#define DESCENSION_ENTITYTYPES_H_

namespace Descension
{
    enum EntityTypes
    {
        E_CAMERA,
        E_UICAMERA,
        E_LIGHTPROJECTOR,
        E_GESTUREINTERPRETER,
        E_GMESSAGE_FAIL,
        E_GMESSAGE_NOMP,
        E_GMESSAGE_STRAIGHT,
        E_GMESSAGE_INTERACT,
        E_UI,
        E_UI_SPARK_PARTICLE,
        E_CURSOR,
        E_BACKBUTTON,
        E_LEVELSELECT,
        E_SPIRITOFDEMAE,
        E_CINEMATICUI,
        E_MAINMENUUI,
        E_OPTIONSUI,
        E_CREDITSUI,
        E_HOWTOPLAYUI,
        E_DESTRUCTIVEUI,
        E_DESTRUCTIVEUINEWGAME,
        E_GAMEOVERUI,
        E_TILEMAP,
        E_PLAYER,
        E_PLAYERMARKER,
		E_PLAYER_CINE,
        E_MAJEN_CINE,
        E_PLAYERBULLET,
        E_ELECTROBULLET,
        E_ELECTROBULLET_ABYSSAL,
        E_BOUDLERPROJECTILE,
        E_FIREBALL,
        E_ROCKDEBRIS,
        E_SHADOW,
        E_ROCKBREAKEFFECT,
        E_POUNDEFFECT,
        E_FIRESPINEFFECT,
        E_FIRETORNADOEFFECT,
        E_MAJENBODYFIRE,
        E_MAJENCLONEEFFECT,
        E_MAJENCHARGEAURA,
        E_FIREBALLEXPLOSION,
        E_FIREBARRELEXPLOSION,
        E_DURGLEEGGPROJECTILE,
        E_DURGLEEGGCHILD,
        E_CRYSTALSWAP,
        E_PARAGONCRYSTAL_ATTACHMENT,
        E_FIRECRYSTAL_ATTACHMENT,
        E_LIGHTNINGCRYSTAL_ATTACHMENT,
        E_ENEMYRANDSPAWN,
        E_SMOKEPUFF,
        E_ENEMYBULLET,
        E_ENEMYFIREBULLET,
        E_FIRESOUNDLOOPEMITTER,
        E_ROOM,
        E_FLOOR,
        E_WALL,
        E_WALL2,
        E_WALL4,
        E_JUKEBOX,
        E_LASERSTONESTATUE,
        E_TRAPFIRESPOT,
        SMALL_WATEREFFECT,
        E_PUZZLE_LEVEL_3_TRIGGER,
        E_PUZZLE_LEVEL_4_TRIGGER,
        E_PUZZLE_LEVERONPLATFORM,
        E_PUZZLE_LEVERONPLATFORM_WATEREFFECT,
        E_PUZZLE_LEVERONPLATFORM_2,
        E_PUZZLE_LEVERONPLATFORM_2_WATEREFFECT,
        E_PUZZLE_BARRELONPLATFORM,
        E_PUZZLE_BARRELONPLATFORM_2,
        E_TOXIC_GAS_PARTICLES,
        E_LEVER_PARTICLES,
        E_ENEMYKALAMANDER,
        E_ENEMYDURGLEMANDER,
        E_ENEMYBASICMELEE,
        E_ENEMYBASICMELEE_TUTORIAL_1,
        E_ENEMYBASICDASHER,
        E_ENEMYBASICCIRCLER,
        E_ENEMYBASICRANGED,
        E_ENEMYBASICRANGED_TUTORIALSHOOTER,
        E_ENEMYBASICCASTER,
        E_ENEMYABYSSALREVENANT,
        E_ENEMYCRYSTALGUARDIAN,
        E_ENEMYCRYSTALGUARDIAN_CINE,
        E_ENEMYCRYSTALGUARDIANFIST,
        E_ENEMYBIG,
        E_ENEMYBOSSGHOST,
        E_ENEMYBOSSGHOST_CINE,
        E_ENEMYMAJEN,
        E_ENEMYSPAWNSPOT,
        E_MAJENTRIANGLE,
        E_SWORD_1,
        E_GUN_1,
        E_PLAYER_LIGHT_BEAMS,
        E_HUMAN_SKELETON_1,
        E_HUMAN_SKELETON_2,
        E_HIGHTOWER_TEXT,
        E_EXITPORTAL,

        E_HEALTHSHARD,
        E_MANASHARD,
        E_SPELLSHARD,
        E_SPELLSHARDFIRE,
        E_SPELLSHARDLIGHTNING,
        E_SPELLSHARD_SPAWNER,
        E_HEALTHCRYSTALORB,
        E_MANACRYSTALORB,
        E_HEALTHCRYSTALORB_NA,
        E_MANACRYSTALORB_NA,
        E_EXPCRYSTALORB_NA,
        E_PROP_SCROLL,
        E_PROP_SCROLLFLAT,
        E_PROP_SCROLLFLAT_3,
        E_PROP_SHRINE,
        E_PROP_SHRINE_TELESLASH_USED,
        E_PROP_SHRINE_FIRE,
        E_PROP_SHRINE_FIRE_USED,
        E_PROP_SHRINE_LIGHTNING,
        E_PROP_SHRINE_LIGHTNING_USED,
        E_PROP_SPELL_TELESLASH,
        E_PROP_SPELL_LIGHTNING,
        E_PROP_SCAFFOLDING_DESTRUCTIBLE,
        E_PROP_DESCENDINGWALL,
        E_PROP_BOAT,
        E_PROP_PARAGONCRYSTAL,
        E_BARREL_LIGHTNING,
        
        E_LIGHTNINGCAST,
        E_GROUNDSMASHCRATER,
        E_LIGHTNINGEFFECT,
        E_BOSSGHOSTLIGHTNING,
        E_LEVELUP_EFFECT,
        E_FLAMEEFFECT,
        E_FLAMINGSNAKEEFFECT,
        E_REVIVEBEAMEFFECT,
        E_GHOSTLIGHTNINGBEAM,
        E_ASHFADEEFFECT,
        E_DEBRISEFFECT,
        E_DUSTEFFECT,
        E_QUAKEDUST,
        E_GROUNDSMASHEFFECT,
        E_DUSTSHOCKWAVEEFFECT,
        E_REGENHEALTHPARTICLES,
        E_REGENMANAPARTICLES,
        E_REGENEXPPARTICLES,
        E_TELESLASH_RECEIVE_PARTICLES,
        E_TELESLASH_RECEIVE_PARTICLES2,
        E_LIGHTNING_RECEIVE_PARTICLES,
        E_TELEPORTEFFECT,
        E_EMBERSEFFECT,
        E_HITEFFECT,
		E_CRYSTALGUARDIANERUPTEFFECT,
		E_CRYSTALGUARDIANDIEFFECT,

        E_GREENLIGHT,
        E_REDLIGHT,
        E_BLUELIGHT,
        E_YELLOWLIGHT,
        E_WHITELIGHT,

        E_TILESET_1_FLOOR_1,
        E_TILESET_1_FLOOR_2,
        E_TILESET_1_FLOOR_3,
        E_TILESET_1_FLOOR_4,
        E_TILESET_1_FLOOR_SIDE_1,
        E_TILESET_1_FLOOR_SIDE_2,
        E_TILESET_1_FLOOR_SIDE_3,
        E_TILESET_1_FLOOR_SIDE_4,
        E_TILESET_1_FLOOR_SIDE_5,
        E_TILESET_1_FLOOR_SIDE_6,
        E_TILESET_1_FLOOR_SIDE_7,
        E_TILESET_1_FLOOR_SIDE_6_PUZZLE_LEVER,
        E_TILESET_1_FLOOR_SIDE_7_PUZZLE_LEVER,
        E_TILESET_1_FLOOR_SIDE_6_PUZZLE_LEVER_2,
        E_TILESET_1_FLOOR_SIDE_7_PUZZLE_LEVER_2,
        E_TILESET_1_FLOOR_TRANSITION_1,
        E_TILESET_1_FLOOR_TRANSITION_2,
        E_TILESET_1_FLOOR_TRANSITION_3,
        E_TILESET_1_FLOOR_TRANSITION_4,
        E_TILESET_1_FLOOR_TRANSITION_5,

        E_TILESET_1_WALL_1,
        E_TILESET_1_WALL_2,
        E_TILESET_1_WALL_3,
        E_TILESET_1_WALL_4,
        E_TILESET_1_WALL_5,
        E_TILESET_1_WALL_6,
		E_TILESET_1_WALL_1_ETHEREAL,
        E_TILESET_1_WALL_2_ETHEREAL,
        E_TILESET_1_WALL_3_ETHEREAL,
        E_TILESET_1_WALL_CORNER_1,
        E_TILESET_1_WALL_DOOR_1,
		E_TILESET_1_WALL_DOOR_1_ETHEREAL,
        E_TILESET_1_WALL_DOOR_2,
        E_TILESET_1_WALL_TRANSITION_1,
        E_TILESET_1_WALL_TRANSITION_2,
        E_TILESET_1_WALL_TRANSITION_3,
        E_TILESET_1_WALL_TRANSITION_4,
        E_TILESET_1_WALL_TRANSITION_5,
        E_TILESET_1_WALL_TRANSITION_6,
        E_TILESET_1_WALL_TRANSITION_7,
        E_TILESET_1_WALL_TRANSITION_8,
        E_TILESET_1_WALL_4_DODGE,
        E_TILESET_1_CLIFF_WALL_1,
        E_TILESET_1_CLIFF_WALL_2,
        E_TILESET_1_CLIFF_WALL_3,
        E_TILESET_1_CLIFF_WALL_4,
        E_TILESET_1_CLIFF_WALL_BEND,
        E_TILESET_1_CLIFF_WALL_TRANSITION_1,
        E_TILESET_1_CLIFF_WALL_TRANSITION_2,
        E_TILESET_1_CLIFF_WALL_TRANSITION_3,
        E_TILESET_1_CLIFF_WALL_TRANSITION_SIDE,

        E_TILESET_1_BRIDGE,
        E_TILESET_1_BRIDGEEND,
        E_TILESET_1_BRIDGETILE,
        E_TILESET_1_PROPCHAINBARRIER,
        E_TILESET_1_PROPBARREL_DESTRUCTIBLE_1,
        E_TILESET_1_PROPBARREL_2,
        E_TILESET_1_PROPCRATE_DESTRUCTIBLE_1,
        E_TILESET_1_PROPBARREL_FIRE_DESTRUCTIBLE,
        E_TILESET_1_CARTOVERTURNED,
        E_TILESET_1_CARTFULL,
        E_TILESET_1_DECALFLOORCRACKS,
        E_TILESET_1_DECALBLOODSTAIN,
        E_TILESET_1_DECALSKIDMARKS_1,
        E_TILESET_1_DECALSKIDMARKS_2,
        E_TILESET_1_DECALMOSS_LONG_1,
        E_TILESET_1_DECALMOSS_LONG_2,
        E_TILESET_1_DECALMOSS_LONG_3,
        E_TILESET_1_DECALMOSS_SHORT_1,
        E_TILESET_1_DECALMOSS_SHORT_2,
        E_TILESET_1_DECALMOSS_SHORT_3,
        E_TILESET_1_DECALGROUND,
        E_TILESET_1_DECALGROUND_2,
        E_TILESET_1_DECALCLIFFEDGE,
        E_TILESET_1_ANTHILL,
        E_TILESET_1_WELL,
        E_TILESET_1_TREE_1,
        E_TILESET_1_TREE_2,
        E_TILESET_1_GATEWAY,
        E_TILESET_1_TUNNEL,
        E_TILESET_1_PROPSTALAGMITE,
        E_TILESET_1_ENVSTALAGMITE,
        E_TILESET_1_PROPDIRTMOUND,
        E_TILESET_1_ENVDIRTMOUND,
        E_TILESET_1_PROPWOODENPLANK,
        E_TILESET_1_PROPREGENHEALTHCRYSTAL,
        E_TILESET_1_PROPREGENMANACRYSTAL,
        E_TILESET_1_PROPCRYSTAL_1,
        E_TILESET_1_PROPCRYSTAL_2,
        E_TILESET_1_PROPCRYSTAL_3,
        E_TILESET_1_PROPCRYSTAL_GREEN_3,
        E_TILESET_1_PROPCRYSTAL_RED_1,
        E_TILESET_1_PROPCRYSTAL_RED_2,
        E_TILESET_1_PROPCRYSTAL_RED_3,
        E_TILESET_1_PROPWALLTORCH,
        E_TILESET_1_PROPGLASSBOTTLE_1,
        E_TILESET_1_PROPGLASSBOTTLE_2,
        E_TILESET_1_PROPMETALSTICK_1,
        E_TILESET_1_PROPMETALSTICK_2,
        E_TILESET_1_PROPLANTERN,
        E_TILESET_1_PROPSALTAR,
        E_TILESET_1_PROPSBRAZIER,
        E_TILESET_1_PROPSDOOR,
		E_TILESET_1_PROPSDOOR_ETHEREAL,
        E_TILESET_1_PROPSFENCE_1,
        E_TILESET_1_PROPSFENCE_2,
        E_TILESET_1_PROPSGEMBLUE,
        E_TILESET_1_PROPSGEMGREEN,
        E_TILESET_1_PROPSGEMRED,
        E_TILESET_1_PROPSLEVERFLOOR,
        E_TILESET_1_PROPSLEVERWALL,
        E_TILESET_1_PROPSPILLAR_1,
        E_TILESET_1_PROPSPILLAR_2,
        E_TILESET_1_PROPSPILLAR_BROKEN,
		E_TILESET_1_PROPSPILLAR_2_ETHEREAL,
        E_TILESET_1_PROPSPILLAR_BROKEN_ETHEREAL,
        E_TILESET_1_PROPSROCK_1,
        E_TILESET_1_PROPSROCK_2,
        E_TILESET_1_PROPSTRAINTRACKS_1,
        E_TILESET_1_PROPSTRAINTRACKS_2,
        E_TILESET_1_PROPSTRAINTRACKS_BEND,
        E_TILESET_1_PROPSTREASURECHEST_CLOSED,
        E_TILESET_1_PROPSTREASURECHEST_OPEN,
        E_TILESET_1_PROPWATERFALL,
        E_TILESET_1_PROPWATERFALLWATER,
        E_TILESET_1_PROPWATERFALLWATER_2,
        E_TILESET_1_PROPWATERFALLWATER_TRANSITION,
        E_TILESET_1_PROPWATERFALLBASE,
        E_TILESET_1_WATER_1,
        E_TILESET_1_WATER_2,


        E_TILESET_2_CLIFFWALL_1,
        E_TILESET_2_CLIFFWALL_2,
        E_TILESET_2_CLIFFWALL_3,
        E_TILESET_2_CLIFFWALL_BEND_1,
        E_TILESET_2_CLIFFWALL_BEND_2,
        E_TILESET_2_CLIFFWALL_BEND_TRANSIT_1,
        E_TILESET_2_CLIFFWALL_BEND_TRANSIT_2,
        E_TILESET_2_CLIFFWALL_TRANSIT_1,
        E_TILESET_2_CLIFFWALL_TRANSIT_2,
        E_TILESET_2_CLIFFWALL_TRANSIT_3,

        E_TILESET_2_DECALDIRT,
        E_TILESET_2_DECALGRASS,
        E_TILESET_2_DIRTTRACKS_1,
        E_TILESET_2_DIRTTRACKS_2,
        E_TILESET_2_DIRTTRACKS_3,
        E_TILESET_2_DIRTTRACKS_4,
        E_TILESET_2_DIRTTRACKS_5,
        E_TILESET_2_DIRTTRACKS_6,
        E_TILESET_2_PROPBED,
        E_TILESET_2_PROPBIGTENT,
        E_TILESET_2_PROPBOAT_1,
        E_TILESET_2_PROPBOAT_2,
        E_TILESET_2_PROPCABINET_1,
        E_TILESET_2_PROPCABINET_2,
        E_TILESET_2_PROPCARPETBIG_1,
        E_TILESET_2_PROPCARPETBIG_2,
        E_TILESET_2_PROPCARPETSMALL,
        E_TILESET_2_PROPCHAIR,
        E_TILESET_2_PROPCHAIR_FALLEN,
        E_TILESET_2_PROPCRYSTAL_1,
        E_TILESET_2_PROPCRYSTAL_2,
        E_TILESET_2_PROPCRYSTAL_3,
        E_TILESET_2_YELLOW_PROPCRYSTAL_1,
        E_TILESET_2_YELLOW_PROPCRYSTAL_2,
        E_TILESET_2_YELLOW_PROPCRYSTAL_3,
        E_TILESET_2_PROPFIREPLACE,
        E_TILESET_2_PROPGRASS_1,
        E_TILESET_2_PROPGRASS_2,
        E_TILESET_2_PROPLADDER,
        E_TILESET_2_PROPPICKAXE,
        E_TILESET_2_PROPPIPE_BEND,
        E_TILESET_2_PROPPIPE_LONG,
        E_TILESET_2_PROPPIPE_MED,
        E_TILESET_2_PROPPIPE_SHORT,
        E_TILESET_2_PROPRIVERDOCK,
        E_TILESET_2_PROPSANDMOUND,
        E_TILESET_2_PROPSCAFFOLDING_BOTTOM,
        E_TILESET_2_PROPSCAFFOLDING_TOP,
        E_TILESET_2_PROPSMALLTENT_1,
        E_TILESET_2_PROPSMALLTENT_2,
        E_TILESET_2_TENTFIRE,
        E_TILESET_2_TENTSMOKE,
        E_TILESET_2_PROPSTOOL,
        E_TILESET_2_PROPTABLE,
        E_TILESET_2_PROPWATERTANK,
        E_TILESET_2_PROPWATERTANK_BROKEN,
		E_TILESET_2_PROPCLOTHESLINE_1,
		E_TILESET_2_PROPCLOTHESLINE_2,
		E_TILESET_2_PROPCLOTHESBASKET,
		
        E_TILESET_2_ROCKYGROUND_1,
        E_TILESET_2_ROCKYGROUND_2,
        E_TILESET_2_ROCKYGROUND_3,
        E_TILESET_2_ROCKYGROUND_4,
        E_TILESET_2_ROCKYSOFTGROUND_1,
        E_TILESET_2_ROCKYSOFTGROUND_2,
        E_TILESET_2_ROCKYSOFTGROUND_3,
        E_TILESET_2_ROCKYSOFTGROUND_4,
        E_TILESET_2_ROCKYSOFTGROUND_XL_TRANSIT_1,
        E_TILESET_2_ROCKYSOFTGROUND_XL_TRANSIT_2,
        E_TILESET_2_ROCKYSOFTGROUND_XL_TRANSITBEND_1,
        E_TILESET_2_ROCKYSOFTGROUND_XL_TRANSITBEND_2,
        E_TILESET_2_ROCKYSOFTGROUND_XR_TRANSIT_1,
        E_TILESET_2_ROCKYSOFTGROUND_XR_TRANSIT_2,
        E_TILESET_2_ROCKYSOFTGROUND_XR_TRANSITBEND_1,
        E_TILESET_2_ROCKYSOFTGROUND_XR_TRANSITBEND_2,
        E_TILESET_2_ROCKYSOFTGROUND_ZL_TRANSIT_1,
        E_TILESET_2_ROCKYSOFTGROUND_ZL_TRANSIT_2,
        E_TILESET_2_ROCKYSOFTGROUND_ZR_TRANSIT_1,
        E_TILESET_2_ROCKYSOFTGROUND_ZR_TRANSIT_2,
        E_TILESET_2_ROCKYSOFTGROUND_ZX_TRANSITBEND_1,
        E_TILESET_2_ROCKYSOFTGROUND_ZX_TRANSITBEND_2,
        E_TILESET_2_ROCKYSOFTGROUND_ZX_TRANSITBEND_3,
        E_TILESET_2_ROCKYSOFTGROUND_ZX_TRANSITBEND_4,
        E_TILESET_2_SOFTGROUND_WATEREDGE_1,
        E_TILESET_2_SOFTGROUND_WATEREDGE_2,
        E_TILESET_2_SOFTGROUND_WATEREDGE_3,
        E_TILESET_2_SOFTGROUND_WATEREDGE_4,
        E_TILESET_2_SOFTGROUND_WATEREDGE_DIAGONAL,
        E_TILESET_2_SOFTGROUND_WATEREDGE_TURN,
        E_TILESET_2_WATERTILE_1,
        E_TILESET_2_WATERTILE_2,



        E_EDITOR,
        E_TOTAL
    };
}

#endif
