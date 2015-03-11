/*************************************************************************/
/*!
\file		GCSpriteEditor.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCSpriteEditor.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/DataFileIO.h"
#include "Simian/FileSystem.h"
#include "Simian/LogManager.h"
#include "Simian/Debug.h"

#include <fstream>

namespace Descension
{
    const Simian::f32 GCSpriteEditor::UV_EPSILON = 0.001f;
    const std::string GCSpriteEditor::UV_DIRECTORY = "UV/";

    GCSpriteEditor* GCSpriteEditor::instance_ = NULL;

    Simian::FactoryPlant<Simian::EntityComponent, GCSpriteEditor> GCSpriteEditor::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_SPRITEEDITOR);

    GCSpriteEditor::GCSpriteEditor()
    {
        //--nothing
    }

    GCSpriteEditor& GCSpriteEditor::Instance()
    {
        return *instance_;
    }

    void GCSpriteEditor::GetUVFromFile
        (const std::string& filePath, std::vector<std::vector<Simian::Vector2> >&
        uiFrames, Simian::u32 sizeExpected, bool useEpsilonPadding)
    {
        //--Get docPath and save/read
        std::string docPath(UV_DIRECTORY);
        docPath += filePath;
        Simian::DataLocation dataLoc(docPath);
        Simian::DataFileIODefault XMLReader;
        XMLReader.Read(dataLoc);
        const Simian::FileObjectSubNode* root = XMLReader.Root();

        std::ifstream fstr(docPath);
        if (!fstr.is_open())
        {
            SError("UV Deserializer: Cannot find file named: " << docPath);
            SAssert(false, "GCSpriteEditor: Cannot find needed file.");
        }
        fstr.close();

        const Simian::FileArraySubNode* uv = root->Array("UI");
        std::vector<Simian::Vector2> insertVec;
        Simian::Vector2 uvVector;
        Simian::f32 readVal = 0.0f,
            padding = useEpsilonPadding ? UV_EPSILON : 0.0f;

        if (uv->Size() < sizeExpected)
        {
            SError("UV Deserializer: Expected frame size not met from: " << docPath);
            SAssert(false, "GCSpriteEditor: FrameSize mismatch, unexpected behaviour may occur.");
        }

        for (Simian::u32 i=0; i<uv->Size(); ++i)
        {
            const Simian::FileObjectSubNode* node = uv->Object(i);
            node->Data("BL_U", readVal);
            uvVector.X(readVal + padding);
            node->Data("BL_V", readVal);
            uvVector.Y(readVal - padding);
            insertVec.push_back(uvVector);

            node->Data("BR_U", readVal);
            uvVector.X(readVal - padding);
            node->Data("BR_V", readVal);
            uvVector.Y(readVal - padding);
            insertVec.push_back(uvVector);

            node->Data("TR_U", readVal);
            uvVector.X(readVal - padding);
            node->Data("TR_V", readVal);
            uvVector.Y(readVal + padding);
            insertVec.push_back(uvVector);

            node->Data("TL_U", readVal);
            uvVector.X(readVal + padding);
            node->Data("TL_V", readVal);
            uvVector.Y(readVal + padding);
            insertVec.push_back(uvVector);

            uiFrames.push_back(insertVec);
            insertVec.clear();
        }

        ////UV: BL BR TR TL
        //std::vector<Simian::Vector2> temp;
        ////--BG
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.5f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.5f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.25f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.25f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--HP
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.0625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.0625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.0f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.0f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--MP
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.0625f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.0625f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--EXP
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.0625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.0625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.0f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.0f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--FLASH
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.1875f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.1875f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--BTN_BG
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.75f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f-UV_EPSILON, 0.75f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f-UV_EPSILON, 0.5f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.5f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--PAUSE
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f-UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f-UV_EPSILON, 0.1875f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.1875f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--OPTION
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.0625f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.0625f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--HOWTOPLAY
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.0625f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.0625f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--QUIT
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.125f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.125f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--RESUME
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.125f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.125f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--RESTART
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.5625f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.5625f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--YES
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f-UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f-UV_EPSILON, 0.125f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.0f+UV_EPSILON, 0.125f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--NO
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.1875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.125f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.125f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();

        ////--OPTION_DOWN
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.1875f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.1875f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--HOWTOPLAY_DOWN
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.25f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.1875f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.1875f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--QUIT_DOWN
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.3125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.3125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.25f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.25f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--RESUME_DOWN
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.3125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.3125f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.25f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.25f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--RESTART_DOWN
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.6875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.6875f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.625f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.625f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--WINQUIT
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.5f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.5f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.375f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.375f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--WIN_RESTART
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.5f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.5f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.375f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON, 0.375f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--YES_DOWN
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.375f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.375f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f-UV_EPSILON, 0.3125f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.3125f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--NO_DOWN
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON,0.375f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.375f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f-UV_EPSILON, 0.3125f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.75f+UV_EPSILON,0.3125f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();

        ////--UI_ICON_SLASH
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.3125f-UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.3125f-UV_EPSILON, 0.5f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.25f+UV_EPSILON, 0.5f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--UI_ICON_LIGHTNING
        //temp.push_back(Simian::Vector2(0.3125f+UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.375f-UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.375f-UV_EPSILON, 0.5f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.3125f+UV_EPSILON, 0.5f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--UI_ICON_FIRE
        //temp.push_back(Simian::Vector2(0.375f+UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.4375f-UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.4375f-UV_EPSILON, 0.5f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.375f+UV_EPSILON, 0.5f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
        ////--UI_ICON_HEAL
        //temp.push_back(Simian::Vector2(0.4375f+UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.5625f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f-UV_EPSILON, 0.5f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.4375f+UV_EPSILON, 0.5f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();

        ////--BOSS0
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.67f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f, 0.67f-UV_EPSILON));
        //temp.push_back(Simian::Vector2(1.0f, 0.5f+UV_EPSILON));
        //temp.push_back(Simian::Vector2(0.5f+UV_EPSILON, 0.5f+UV_EPSILON));
        //uiFrames.push_back(temp);
        //temp.clear();
    }

    void GCSpriteEditor::update_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
    }

    void GCSpriteEditor::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCSpriteEditor, &GCSpriteEditor::update_>(this));
    }

    void GCSpriteEditor::OnAwake()
    {
        instance_ = this;
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
    }

    void GCSpriteEditor::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        //--nothing
    }

    void GCSpriteEditor::Serialize( Simian::FileObjectSubNode& )
    {
        //--nothing
    }

    void GCSpriteEditor::Deserialize( const Simian::FileObjectSubNode& )
    {
        //--nothing
    }
}
