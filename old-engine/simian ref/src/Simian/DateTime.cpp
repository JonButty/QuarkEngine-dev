/************************************************************************/
/*!
\file		DateTime.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/DateTime.h"
#include "Simian/OS.h"

#include <sstream>
#include <iomanip>

namespace Simian
{
    static const std::string MONTH_TABLE[] =
    {
        "Non",
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };

    DateTime::DateTime(bool localTime)
        : day_(1),
          month_(DTM_JANUARY),
          year_(0),
          hour_(0),
          minute_(0),
          second_(0)
    {
        u8 month;
        if (localTime)
            OS::Instance().CurrentLocalDateTime(day_, month, year_, hour_, minute_, second_);
        else
            OS::Instance().CurrentDateTime(day_, month, year_, hour_, minute_, second_);
        month_ = static_cast<DTMonth>(month);
    }

    DateTime::DateTime( u8 day, DTMonth month, u16 year, u8 hour, u8 minute, u8 second )
        : day_(day),
          month_(month),
          year_(year),
          hour_(hour),
          minute_(minute),
          second_(second)
    {
    }

    Simian::u8 DateTime::Day() const
    {
        return day_;
    }

    void DateTime::Day( Simian::u8 val )
    {
        day_ = val;
    }

    Simian::DateTime::DTMonth DateTime::Month() const
    {
        return month_;
    }

    void DateTime::Month( DTMonth val )
    {
        month_ = val;
    }

    Simian::u16 DateTime::Year() const
    {
        return year_;
    }

    void DateTime::Year( Simian::u16 val )
    {
        year_ = val;
    }

    Simian::u8 DateTime::Hour() const
    {
        return hour_;
    }

    void DateTime::Hour( Simian::u8 val )
    {
        hour_ = val;
    }

    Simian::u8 DateTime::Minute() const
    {
        return minute_;
    }

    void DateTime::Minute( Simian::u8 val )
    {
        minute_ = val;
    }

    Simian::u8 DateTime::Second() const
    {
        return second_;
    }

    void DateTime::Second( Simian::u8 val )
    {
        second_ = val;
    }


    std::string DateTime::ToString() const
    {
        std::stringstream ss;
        ss << MONTH_TABLE[month_] << " " << std::setfill('0') << std::setw(2) <<
            static_cast<int>(day_) << ", " << std::setw(4) << year_ << " | " << std::setw(2) << static_cast<int>(hour_)
            << ":" << std::setw(2) << static_cast<int>(minute_) << ":" << std::setw(2) << static_cast<int>(second_);
        return ss.str();
    }
}
