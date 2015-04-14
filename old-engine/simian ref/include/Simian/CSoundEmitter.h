/*****************************************************************************/
/*!
\file		CSoundEmitter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_CSOUNDEMITTER_H_
#define SIMIAN_CSOUNDEMITTER_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class CTransform;
    class Sound;

    class SIMIAN_EXPORT CSoundEmitter: public EntityComponent
    {
    private:
        struct SoundElement
        {
            std::string Location;
            f32 Volume;

            bool operator==(const std::string& name)
            {
                return name == Location;
            }
        };
    private:
        CTransform* transform_;
        bool is3D_;

        std::vector<SoundElement> soundElements_;
        std::vector<Sound*> sounds_;

        static FactoryPlant<EntityComponent, CSoundEmitter> factoryPlant_;
    public:
        bool SoundId(u32& soundId, const std::string& soundName);
        const std::string& SoundName(u32 soundId);
    public:
        CSoundEmitter();

        void OnAwake();

        void Play(u32 soundId, f32 volume = -1.0f);
        void Play(const std::string& soundName, f32 volume = -1.0f);
        void Stop(u32 soundId);
        void StopAll();

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
