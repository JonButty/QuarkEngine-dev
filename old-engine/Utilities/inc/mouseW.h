#ifndef MOUSEW_H
#define MOUSEW_H

#include "inputDevice.h"
#include "vector2.h"
#include "configUtilitiesDLL.h"
#include "inputDeviceFactory.h"

namespace Util
{
    class ENGINE_UTILITIES_EXPORT Mouse
        :   public InputDevice
    {
    public:
        Mouse(unsigned int maxKeys);
        ~Mouse();
        Math::Vec2I ClientPosition() const;
        void ClientPosition(const Math::Vec2I& val);
        Math::Vec2I MouseDelta() const;
    public:
        int Wheel() const;
        Math::Vec2I ScreenPosition() const;
        void ScreenPosition(const Math::Vec2I& val);
    protected:
        void update_(const UpdateInfo& info);
        unsigned int getMappedKeyValue_(unsigned int val) const;
    private:
        int wheel_;
        Math::Vec2I prevScreenPos_;
        Math::Vec2I currScreenPos_;
    };
}
#endif