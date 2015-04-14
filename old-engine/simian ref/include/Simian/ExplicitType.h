/************************************************************************/
/*!
\file		ExplicitType.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_EXPLICITTYPE_H_
#define SIMIAN_EXPLICITTYPE_H_

namespace Simian
{
    template <class T>
    class ExplicitType
    {
    private:
        T data_;
    public:
        // no copies here!
        const ExplicitType& operator=(const ExplicitType&) const
        {
            return *this;
        }

        T& operator->()
        {
            return data_;
        }

        const T& operator->() const
        {
            return data_;
        }

        T& operator *()
        {
            return data_;
        }

        operator T() const
        {
            return data_;
        }
    public:
        ExplicitType()
        {
        }

        ExplicitType(const T& value)
            : data_(value)
        {
        }

        void Set(const T& value)
        {
            data_ = value;
        }
    };
}

#endif
