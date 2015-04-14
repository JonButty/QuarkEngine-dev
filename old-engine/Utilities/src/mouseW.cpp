#include "mouseW.h"
#include "os.h"

#include "Winuser.h"

namespace Util
{
    Mouse::Mouse(unsigned int maxKeys)
        :   InputDevice(ID_MOUSE,maxKeys),
            wheel_(0),
            prevScreenPos_(Math::Vec2I::Zero),
            currScreenPos_(Math::Vec2I::Zero)
    {
    }

    Mouse::~Mouse()
    {
    }

    Math::Vec2I Mouse::ClientPosition() const
    {
        POINT tempPoint;
        tempPoint.x = currScreenPos_.X();
        tempPoint.y = currScreenPos_.Y();

        WindowHandle handle = OS::Instance().Handle();

        ScreenToClient(handle, &tempPoint);
        return Math::Vec2I(tempPoint.x, 
                           tempPoint.y);
    }

    void Mouse::ClientPosition( const Math::Vec2I& val )
    {
        POINT tempPoint;
        tempPoint.x = val.X();
        tempPoint.y = val.Y();

        WindowHandle handle = OS::Instance().Handle();

        ClientToScreen(handle, &tempPoint);
        ScreenPosition(Math::Vec2I(tempPoint.x,
                                   tempPoint.y));

    }

    Math::Vec2I Mouse::MouseDelta() const
    {
        return currScreenPos_ - prevScreenPos_;
    }

    //--------------------------------------------------------------------------

    int Mouse::Wheel() const
    {
        return wheel_;
    }

    Math::Vec2I Mouse::ScreenPosition() const
    {
        return currScreenPos_;
    }

    void Mouse::ScreenPosition( const Math::Vec2I& val )
    {
        currScreenPos_ = val;
        SetCursorPos(val.X(),val.Y());
    }

    //--------------------------------------------------------------------------

    void Mouse::update_(const UpdateInfo& info)
    {
        prevScreenPos_ = currScreenPos_;

        POINT pt;
        GetCursorPos(&pt);
        currScreenPos_.X(pt.x);
        currScreenPos_.Y(pt.y);

        short delta = GET_WHEEL_DELTA_WPARAM(info.wParam_);
        wheel_ = delta;
    }

    unsigned int Mouse::getMappedKeyValue_(unsigned int val) const
    {
        return val;
    }
}