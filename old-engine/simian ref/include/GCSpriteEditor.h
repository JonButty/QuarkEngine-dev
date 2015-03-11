/*************************************************************************/
/*!
\file		GCSpriteEditor.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCSPRITEEDITOR_H_
#define DESCENSION_GCSPRITEEDITOR_H_

#include "Simian/Vector2.h"
#include "Simian/SimianTypes.h"
#include "Simian/EntityComponent.h"

#include <vector>
#include <string>

namespace Descension
{
    class GCSpriteEditor : public Simian::EntityComponent
    {
    public:
        static const Simian::f32 UV_EPSILON;
        static const std::string UV_DIRECTORY;

    private:
        static GCSpriteEditor* instance_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCSpriteEditor>
            factoryPlant_;

    public:
        static GCSpriteEditor& Instance(void);

        static void GetUVFromFile
            (const std::string& filePath, std::vector<std::vector<Simian::Vector2> >&
            uiFrames, Simian::u32 sizeExpected, bool useEpsilonPadding = true);

    private:
        void update_(Simian::DelegateParameter& param);
    
    public:
        GCSpriteEditor();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
