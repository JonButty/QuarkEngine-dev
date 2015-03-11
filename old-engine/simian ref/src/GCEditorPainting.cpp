/*************************************************************************/
/*!
\file		GCEditorPainting.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCEditor.h"

#include "Simian/Delegate.h"
#include "Simian/Mouse.h"

namespace Descension
{
    struct paintingCallbacks_
    {
        typedef void(GCEditor::*Callback)();

        Callback LButtonDown;
        Callback LButtonUpdate;
        Callback LButtonRelease;
        Callback RButtonDown;
        Callback RButtonUpdate;
        Callback RButtonRelease;
    };
    static paintingCallbacks_ PAINTING_CALLBACKS[GCEditor::EM_TOTAL];
    static void createPaintingCallbacks_()
    {
        {
            paintingCallbacks_ callbacks =
            {
                &GCEditor::collisionModeLButtonStart_,
                &GCEditor::collisionModeLButtonUpdate_,
                &GCEditor::collisionModeLButtonEnd_,
                &GCEditor::collisionModeRButtonStart_,
                &GCEditor::collisionModeRButtonUpdate_,
                &GCEditor::collisionModeRButtonEnd_
            };
            PAINTING_CALLBACKS[GCEditor::EM_COLLISION] = callbacks;
        }

        {
            paintingCallbacks_ callbacks =
            {
                &GCEditor::sceneModeLButtonStart_,
                NULL,
                &GCEditor::sceneModeLButtonEnd_,
                &GCEditor::sceneModeRButtonStart_,
                NULL,
                &GCEditor::sceneModeRButtonEnd_
            };
            PAINTING_CALLBACKS[GCEditor::EM_SCENE] = callbacks;
        }

        {
            paintingCallbacks_ callbacks =
            {
                &GCEditor::terraformModeLButtonStart_,
                &GCEditor::terraformModeLButtonUpdate_,
                &GCEditor::terraformModeLButtonEnd_,
                &GCEditor::terraformModeRButtonStart_,
                &GCEditor::terraformModeRButtonUpdate_,
                &GCEditor::terraformModeRButtonEnd_
            };
            PAINTING_CALLBACKS[GCEditor::EM_TERRAFORM] = callbacks;
        }
    }

    static void DoCallback(GCEditor* editor, paintingCallbacks_::Callback callback)
    {
        if (callback)
            (editor->*callback)();
    }

    void GCEditor::setupPaintingSM_()
    {
        paintingSM_[PE_DEFAULT].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::paintDefaultUpdate_>(this);

        paintingSM_[PE_LBUTTONDOWN].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::lButtonDownEnter_>(this);
        paintingSM_[PE_LBUTTONDOWN].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::lButtonDownUpdate_>(this);
        paintingSM_[PE_LBUTTONUP].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::lButtonUpEnter_>(this);
        paintingSM_[PE_LBUTTONUP].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::lButtonUpUpdate_>(this);

        paintingSM_[PE_RBUTTONDOWN].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::rButtonDownEnter_>(this);
        paintingSM_[PE_RBUTTONDOWN].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::rButtonDownUpdate_>(this);
        paintingSM_[PE_RBUTTONUP].OnEnter = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::rButtonUpEnter_>(this);
        paintingSM_[PE_RBUTTONUP].OnUpdate = Simian::Delegate::CreateDelegate<GCEditor, &GCEditor::rButtonUpUpdate_>(this);

        createPaintingCallbacks_();
    }

    void GCEditor::paintDefaultUpdate_( Simian::DelegateParameter& )
    {
        if (Simian::Mouse::IsPressed(Simian::Mouse::KEY_LBUTTON))
            paintingSM_.ChangeState(PE_LBUTTONDOWN);
        if (Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON))
            paintingSM_.ChangeState(PE_RBUTTONDOWN);
    }

    void GCEditor::lButtonDownEnter_( Simian::DelegateParameter& )
    {
        startPos_ = pickedCoordinates_;
        DoCallback(this, PAINTING_CALLBACKS[modeSM_.CurrentState()].LButtonDown);
    }

    void GCEditor::lButtonDownUpdate_( Simian::DelegateParameter& )
    {
        painted_ = 
            std::floor(startPos_.X()) != std::floor(pickedCoordinates_.X()) || 
            std::floor(startPos_.Y()) != std::floor(pickedCoordinates_.Y()) ? 
            true : painted_;

        DoCallback(this, PAINTING_CALLBACKS[modeSM_.CurrentState()].LButtonUpdate);

        if (Simian::Mouse::IsDepressed(Simian::Mouse::KEY_LBUTTON))
            paintingSM_.ChangeState(PE_LBUTTONUP);
    }

    void GCEditor::lButtonUpEnter_( Simian::DelegateParameter& )
    {
        DoCallback(this, PAINTING_CALLBACKS[modeSM_.CurrentState()].LButtonRelease);

        painted_ = false;
    }

    void GCEditor::lButtonUpUpdate_( Simian::DelegateParameter& )
    {
        paintingSM_.ChangeState(PE_DEFAULT);
    }

    void GCEditor::rButtonDownEnter_( Simian::DelegateParameter& )
    {
        startPos_ = pickedCoordinates_;
        DoCallback(this, PAINTING_CALLBACKS[modeSM_.CurrentState()].RButtonDown);
    }

    void GCEditor::rButtonDownUpdate_( Simian::DelegateParameter& )
    {
        DoCallback(this, PAINTING_CALLBACKS[modeSM_.CurrentState()].RButtonUpdate);

        if (Simian::Mouse::IsDepressed(Simian::Mouse::KEY_RBUTTON))
            paintingSM_.ChangeState(PE_RBUTTONUP);
    }

    void GCEditor::rButtonUpEnter_( Simian::DelegateParameter& )
    {
        DoCallback(this, PAINTING_CALLBACKS[modeSM_.CurrentState()].RButtonRelease);
    }

    void GCEditor::rButtonUpUpdate_( Simian::DelegateParameter& )
    {
        paintingSM_.ChangeState(PE_DEFAULT);
    }
}
