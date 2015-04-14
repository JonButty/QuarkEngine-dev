/*****************************************************************************/
/*!
\file		Simian.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_SIMIAN_H_
#define SIMIAN_SIMIAN_H_

/**
    @mainpage
        This document outlines the functionality and classes of the Simian
        Layer.

    @section Introduction
        The Simian Layer is a code framework of reusable and independent 
        components. Together they can be used as a pseudo game engine, or to
        extend one.

        The components of the Simian Layer are often simple and commonly used
        components of a standard game engine. This includes (but is not limited 
        to): State machines, Design pattern implementation, OS abstracted
        debugging utilities, Math utilities, etc.
*/

// includes all associated headers ;)

// Patterns
#include "Singleton.h"
#include "Observer.h"

// Core
#include "Debug.h"
#include "OS.h"
#include "Window.h"
#include "DateTime.h"
#include "StateMachine.h"
#include "GameStateManager.h"
#include "MemoryPool.h"
#include "FileSystem.h"
#include "SimianPlatform.h"
#include "SimianTypes.h"
#include "Utility.h"
#include "FiniteStateMachine.h"
#include "Delegate.h"

// Graphics
#include "GraphicsDevice.h"
#include "Texture.h"
#include "VertexFormat.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Viewport.h"

// Audio
#include "Audio.h"
#include "AudioManager.h"
#include "Sound.h"
#include "Music.h"

// Math
#include "Vector2.h"
#include "Matrix.h"
#include "Angle.h"
#include "Vector3.h"
#include "Color.h"
#include "Simian/Math.h"
#include "Interpolation.h"

// Components
#include "Entity.h"
#include "EntityComponent.h"
#include "EntityPool.h"
#include "Scene.h"
#include "EntityFactory.h"
#include "GameFactory.h"
#include "DefinitionTable.h"

// more to come...

#endif
