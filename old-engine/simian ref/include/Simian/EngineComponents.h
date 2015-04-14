/************************************************************************/
/*!
\file		EngineComponents.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ENGINECOMPONENTS_H_
#define SIMIAN_ENGINECOMPONENTS_H_

namespace Simian
{
    enum EngineComponents
    {
        C_TRANSFORM,
        C_CAMERA,
        C_ORTHOCAMERA,
        C_MODEL,
        C_MODELATTACHMENT,
        C_MODELCOLOROPERATION,
        C_SPRITE,
        C_BITMAPTEXT,
        C_PARTICLESYSTEM,
        C_PARTICLEEMITTERSPHERE,
        C_PARTICLEEMITTERDISC,
        C_PARTICLERENDERERBILLBOARD,
        C_PARTICLERENDERERRIBBONTRAIL,
        C_PARTICLERENDERERMODEL,
        C_PARTICLETIMELINE,
        C_BONETRAIL,
        C_BILLBOARDROTATE,
        C_UIRESIZE,
        C_JUKEBOX,
        C_SOUNDEMITTER,
        C_SOUNDLISTENER,
        C_VIDEO,
        C_TOTAL
    };
}

#endif
