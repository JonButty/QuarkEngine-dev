/*****************************************************************************/
/*!
\file		OS.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_OS_H_
#define SIMIAN_OS_H_

#include "SimianPlatform.h"
#include "Singleton.h"
#include "Vector2.h"

#include <string>
#include <vector>

namespace Simian
{
    typedef void* WindowHandle;
    class DateTime;

    /** Provides a template for implementation of OS specific functionality.
        @remarks
            Implementation of OSBase are Singletons and can be accessed 
            globally.
    */
    template <class T>
    class SIMIAN_EXPORT OSBase: public Singleton<T>
    {
    public:
        enum SpecialFolder
        {
            SF_USER,
            SF_DOCUMENTS
        };

        enum WindowStyle
        {
            SWS_BORDERS,
            SWS_POPUP
        };
    public:
        /** Stops the execution of the application when expr evaluates to false.
            @remarks
                This method produces the most verbose assert possible for an
                operating system. message must be printed when the execution
                of the application halts.

                Instead of invoking this method directly, use the SAssert macro
                to perform code assertion for automated truncation in release
                builds.
        */
        S_NIMPL virtual void Assert(bool expr, const std::wstring& message) const = 0;

        /** Gets the "My Documents" folder of the operating system. */
        S_NIMPL virtual std::string GetSpecialFolderPath(SpecialFolder folder) const = 0;

        /** Gets the files in a directory. */
        S_NIMPL virtual void GetFilesInDirectory(const std::string& directoryPath, std::vector<std::string>& files) const = 0;

        //----------------------------------------------------------------------
        //
        // Window related functions
        //
        //----------------------------------------------------------------------

        /** Creates a new window. */
        S_NIMPL virtual bool WindowCreate(WindowHandle& window, 
            const Vector2& position, const Vector2& size, const std::string& title,
            WindowStyle style, u32 icon = 0) const = 0;

        /** Updates the window. 
            @remarks
                If this function returns false, the window is not available for
                rendering in that frame, so ignore it.
        */
        S_NIMPL virtual bool WindowUpdate(WindowHandle& window) const = 0;

        /** Destroy the window. */
        S_NIMPL virtual void WindowDestroy(WindowHandle& window) const = 0;

        /** Move the window. */
        S_NIMPL virtual void WindowMove(WindowHandle& window, const Simian::Vector2& position) const = 0;

        /** Resize the window. */
        S_NIMPL virtual void WindowResize(WindowHandle& window, const Simian::Vector2& size) const = 0;

        /** Change window style. */
        S_NIMPL virtual void WindowRestyle(WindowHandle& window, WindowStyle style) const = 0;

        /** Checks if a window is closed. */
        S_NIMPL virtual bool WindowClosed(const WindowHandle& window) const = 0;

        /** Checks if a window is visible. */
        S_NIMPL virtual bool WindowVisible(const WindowHandle& window) const = 0;

        /** Get the current local time and date. */
        S_NIMPL virtual void CurrentLocalDateTime(u8& day, u8& month, u16& year, u8& hour, u8& minute, u8& second) const = 0;

        /** Get the current time and date UTC. */
        S_NIMPL virtual void CurrentDateTime(u8& day, u8& month, u16& year, u8& hour, u8& minute, u8& second) const = 0;

        /** Get the system start up time, accurately. */
        S_NIMPL virtual f32 GetOSTicks() const = 0;

        /** Get the high performance counter. */
        S_NIMPL virtual u64 GetHighPerformanceCounter() const = 0;

        /** Get the high performance counter. */
        S_NIMPL virtual u64 GetHighPerformanceFrequency() const = 0;

        /** Sets the cursor visibility */
        S_NIMPL virtual void CursorSetVisibility(bool visible) const = 0;

        /** Gets the desktop width and height */
        S_NIMPL virtual void DesktopResolution(f32& width, f32& height) const = 0;

        /** Creates a directory if it doesn't exist */
        S_NIMPL virtual void DirectoryCreate(const std::string& filePath) const = 0;
    };
}

// Detects operating system and extends OS according to operating system.
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#include "OSW.h"
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_LINUX

#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_OSX

#endif

#endif
