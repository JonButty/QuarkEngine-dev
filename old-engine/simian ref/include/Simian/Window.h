/************************************************************************/
/*!
\file		Window.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_WINDOW_H_
#define SIMIAN_WINDOW_H_

#include "SimianPlatform.h"
#include "Vector2.h"
#include "OS.h"

#include <string>

namespace Simian
{
    class SIMIAN_EXPORT Window
    {
    private:
        std::string title_;
        Simian::Vector2 position_;
        Simian::Vector2 size_;
        OS::WindowStyle style_;
        WindowHandle handle_;
        unsigned icon_;
    public:
        const std::string& Title() const;

        const Simian::Vector2& Position() const;
        void Position(const Simian::Vector2& val);

        const Simian::Vector2& Size() const;
        void Size(const Simian::Vector2& val);

        OS::WindowStyle Style() const;
        void Style(OS::WindowStyle val);

        const Simian::WindowHandle& Handle() const;

        bool Closed() const;
        bool Visible() const;
    public:
        operator bool() const;
    public:
        Window(const std::string& title, const Simian::Vector2& position, const Simian::Vector2& size, OS::WindowStyle style, unsigned icon = 0);
        ~Window();

        bool Startup();
        void Shutdown();

        bool Update();
    };
}

#endif
