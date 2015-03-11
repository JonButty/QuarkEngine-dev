/************************************************************************/
/*!
\file		Window.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Window.h"

namespace Simian
{
    Window::Window( const std::string& title, const Simian::Vector2& position, const Simian::Vector2& size, OS::WindowStyle style, unsigned icon )
        : title_(title),
          position_(position),
          size_(size),
          style_(style),
          handle_(0),
          icon_(icon)
    {
    }

    Window::~Window()
    {
    }

    //--------------------------------------------------------------------------

    const std::string& Window::Title() const
    {
        return title_;
    }

    const Simian::Vector2& Window::Position() const
    {
        return position_;
    }

    void Window::Position( const Simian::Vector2& val )
    {
        position_ = val;
        OS::Instance().WindowMove(handle_, val);
    }

    const Simian::Vector2& Window::Size() const
    {
        return size_;
    }

    void Window::Size( const Simian::Vector2& val )
    {
        size_ = val;
        OS::Instance().WindowResize(handle_, val);
    }

    OS::WindowStyle Window::Style() const
    {
        return style_;
    }

    void Window::Style( OS::WindowStyle val )
    {
        style_ = val;
        OS::Instance().WindowRestyle(handle_, val);
    }

    const Simian::WindowHandle& Window::Handle() const
    {
        return handle_;
    }

    bool Window::Closed() const
    {
        return OS::Instance().WindowClosed(handle_);
    }

    bool Window::Visible() const
    {
        return OS::Instance().WindowVisible(handle_);
    }

    //--------------------------------------------------------------------------

    Window::operator bool() const
    {
        return !Closed();
    }

    //--------------------------------------------------------------------------

    bool Window::Startup()
    {
        return OS::Instance().WindowCreate(handle_, position_, size_, title_, style_, icon_);
    }

    void Window::Shutdown()
    {
        OS::Instance().WindowDestroy(handle_);
    }

    bool Window::Update()
    {
        return OS::Instance().WindowUpdate(handle_);
    }
}
