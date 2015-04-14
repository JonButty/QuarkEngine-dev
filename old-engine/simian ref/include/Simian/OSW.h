/*****************************************************************************/
/*!
\file		OSW.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_OSW_H_
#define SIMIAN_OSW_H_

#include "SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include "OS.h"

#include <string>

namespace Simian
{
    /** Implements OSBase in a Win32 Environment. 
        @remarks
            You should not access this class directly. All implemented OSBase 
            systems should be accessed through the redefinition "OS" interface. 
            This resolves cross-platform portability automagically.
    */
    class SIMIAN_EXPORT OS: public OSBase<OS>
    {
    public:
        /** See OSBase::Assert */
        void Assert(bool expr, const std::wstring& msg) const;

        /** See OSBase::GetSpecialFolderPath */
        std::string GetSpecialFolderPath(SpecialFolder folder) const;

        void GetFilesInDirectory(const std::string& directoryPath, std::vector<std::string>& files) const;

        bool WindowCreate(WindowHandle& window, const Simian::Vector2& position, const Simian::Vector2& size, const std::string& title, WindowStyle style, u32 icon = 0) const;

        bool WindowUpdate( WindowHandle& window ) const;

        void WindowDestroy( WindowHandle& window ) const;

        void WindowMove( WindowHandle& window, const Simian::Vector2& position ) const;

        void WindowResize( WindowHandle& window, const Simian::Vector2& size ) const;

        void WindowRestyle( WindowHandle& window, WindowStyle style ) const;

        bool WindowClosed( const WindowHandle& window ) const;

        bool WindowVisible(const WindowHandle& window) const;

        void CurrentDateTime(u8& day, u8& month, u16& year, u8& hour, u8& minute, u8& second) const;

        void CurrentLocalDateTime(u8& day, u8& month, u16& year, u8& hour, u8& minute, u8& second) const;

        f32 GetOSTicks() const;

        u64 GetHighPerformanceCounter() const;

        u64 GetHighPerformanceFrequency() const;

        void CursorSetVisibility(bool visible) const;

        void DesktopResolution(f32& width, f32& height) const;

        void DirectoryCreate(const std::string& filePath) const;
    };

    SIMIAN_IMPORT template class SIMIAN_EXPORT OSBase<OS>;
    SIMIAN_IMPORT template class SIMIAN_EXPORT Singleton<OS>;
}

#endif 

#endif
