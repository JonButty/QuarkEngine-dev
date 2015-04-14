/*************************************************************************/
/*!
\file		StateLoading.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_STATELOADING_H_
#define DESCENSION_STATELOADING_H_

#include "Simian/GameStateManager.h"
#include "Simian/Singleton.h"
#include "Simian/Delegate.h"
#include "Simian/Thread.h"
#include "Simian/Vector2.h"
#include "Simian/Color.h"

namespace Simian
{
    class Material;
	class Model;
    class Camera;
    class RenderTexture;
    class Scene;
	class BitmapFont;
	class BitmapText;
}

namespace Descension
{
    class StateLoading: public Simian::GameState, public Simian::Singleton<StateLoading>
    {
    private:
        Simian::Camera* camera_;
        Simian::RenderTexture* renderTexture_;

		// paragon crystal resources
		Simian::Material* crystalMaterial_;
		Simian::Model* crystalModel_;

		// game title resources
		Simian::Material* titleMaterial_;

		// text based resources
		Simian::Material* fontMaterial_;
		Simian::Material* hintBoxMaterial_;
		Simian::BitmapFont* font_;
		Simian::BitmapText* text_;

        // background resources
        Simian::Material* background_;

        Simian::GameState* nextState_;
        Simian::Thread loadingThread_;

        Simian::Scene* scene_;
        Simian::LockedAttribute<bool> loadingLayoutDone_;
        Simian::LockedAttribute<bool> loadingFilesDone_;
        bool loadedResources_;
        Simian::Lock loadingResourcesLock_;

        Simian::Color backgroundColor_;
        Simian::f32 timer_;
        Simian::f32 crystalSpin_;
        Simian::f32 lastTime_;
        bool hideLoadScreen_;
    public:
        void NextState(Simian::GameState* val);
        void BackgroundColor(const Simian::Color& val);
        void Timer(Simian::f32 val);
        void HideLoadScreen(bool val);
    private:
        void loadingThreadEntry_(Simian::DelegateParameter&);
        void progress_(Simian::DelegateParameter&);
        void draw_(Simian::DelegateParameter&);
		void drawText_(Simian::DelegateParameter&);
        void updateSpinner_(Simian::f32 dt);
    public:
        StateLoading();

        void OnEnter();
        void OnLoad();
        void OnInit();
        void OnUpdate(Simian::f32 dt);
        void OnDeinit();
        void OnUnload();
        void OnExit();
    };
}

#endif
