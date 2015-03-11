#include "delegate.h"

namespace Util
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
}