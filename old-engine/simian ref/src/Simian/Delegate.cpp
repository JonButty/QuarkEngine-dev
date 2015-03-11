/************************************************************************/
/*!
\file		Delegate.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Delegate.h"

#include <algorithm>

namespace Simian
{
    DelegateParameter Delegate::NoParameter = DelegateParameter();

    Delegate::Delegate()
        : callingClass_(0),
          staticCallback_(0)
    {
    }

    Delegate::~Delegate()
    {
    }
    
    //--------------------------------------------------------------------------

    void Delegate::operator()( DelegateParameter& arg )
    {
        Call(arg);
    }

    //--------------------------------------------------------------------------

    void Delegate::Call( DelegateParameter& arg )
    {
        if (staticCallback_)
            staticCallback_(callingClass_, arg);
    }

    //--------------------------------------------------------------------------

    void DelegateList::operator()( DelegateParameter& param )
    {
        Call(param);
    }

    void DelegateList::operator+=( const Delegate& delegate )
    {
        AddDelegate(delegate);
    }

    void DelegateList::operator-=( const Delegate& delegate )
    {
        RemoveDelegate(delegate);
    }

    //--------------------------------------------------------------------------

    void DelegateList::Call( DelegateParameter& param )
    {
        for (u32 i = 0; i < delegates_.size(); ++i)
            delegates_[i](param);
    }

    void DelegateList::AddDelegate( const Delegate& delegate )
    {
        delegates_.push_back(delegate);
    }

    void DelegateList::RemoveDelegate( const Delegate& delegate )
    {
        std::vector<Delegate>::iterator i = std::find(delegates_.begin(), 
            delegates_.end(), delegate);
        delegates_.erase(i);
    }

    void DelegateList::Clear()
    {
        delegates_.clear();
    }
}
