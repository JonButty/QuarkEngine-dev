/*************************************************************************/
/*!
\file		GestureTutor.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/11/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/SimianTypes.h"
#include "Simian/Vector2.h"

#include <vector>

namespace Descension {
    //!--Set ID and draw
    class GestureTutor
    {
    public:
        enum GESTURE_TYPE
        {
            GT_SLASH = 0,
            GT_LIGHTNING,
            GT_FIRE,

            GT_TOTAL
        };

        //!--Timer and XY position relative to minor axis (height)
        struct GesturePath
        {
            Simian::f32 Timer;
            Simian::Vector2 Position;
        };

    private:
        static Simian::f32 aniTime_, delayTime_, DELAY_TIME;
        static Simian::u32 drawType_, frameID_; //--Uses Gesture::GCMD_XXXX
        static std::vector<std::vector<GesturePath> > pathVec_;

        static Simian::Vector2 lastPos_;

        static std::string PATH_DIRECTORY;

    public: //--settors & accessors
        static void DrawType(Simian::u32 val);
        static Simian::u32 DrawType(void);

        static void DrawGesture(Simian::f32 dt);

        static void LoadPath();
    };
}
