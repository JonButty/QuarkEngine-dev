/*************************************************************************/
/*!
\file		ParticleEmitterTriangle.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_PARTICLEEMITTERTRIANGLE_H_
#define DESCENSION_PARTICLEEMITTERTRIANGLE_H_

#include "Simian/Vector3.h"
#include "Simian/ParticleEmitter.h"

namespace Descension
{
    class ParticleEmitterTriangle: public Simian::ParticleEmitter
    {
    private:
        Simian::Vector3 vertices_[3];
    public:
        const Simian::Vector3* Vertices() const;
        void Vertices(const Simian::Vector3* val);
    public:
        ParticleEmitterTriangle(Simian::ParticleSystem* parent);

        void Spawn( Simian::Particle& particle );
    };
}

#endif
