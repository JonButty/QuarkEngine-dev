/************************************************************************/
/*!
\file		DateTime.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_DATETIME_H_
#define SIMIAN_DATETIME_H_

#include "SimianPlatform.h"

#include <string>

namespace Simian
{
    class SIMIAN_EXPORT DateTime
    {
    public:
        enum DTMonth
        {
            DTM_JANUARY = 1,
            DTM_FEBRUARY,
            DTM_MARCH,
            DTM_APRIL,
            DTM_MAY,
            DTM_JUNE,
            DTM_JULY,
            DTM_AUGUST,
            DTM_SEPTEMBER,
            DTM_OCTOBER,
            DTM_NOVEMBER,
            DTM_DECEMBER
        };
    private:
        u8 day_;
        DTMonth month_;
        u16 year_;
        u8 hour_, minute_, second_;
    public:
        Simian::u8 Day() const;
        void Day(Simian::u8 val);

        Simian::DateTime::DTMonth Month() const;
        void Month(Simian::DateTime::DTMonth val);

        Simian::u16 Year() const;
        void Year(Simian::u16 val);

        Simian::u8 Hour() const;
        void Hour(Simian::u8 val);

        Simian::u8 Minute() const;
        void Minute(Simian::u8 val);

        Simian::u8 Second() const;
        void Second(Simian::u8 val);
    public:
        DateTime(bool localTime = true);
        DateTime(u8 day, DTMonth month, u16 year, u8 hour, u8 minute, u8 second);

        std::string ToString() const;
    };
}

#endif
