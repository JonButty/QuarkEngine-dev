/******************************************************************************/
/*!
\file		GetterMemFuncPtr.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GETTERMEMFUNCPTR_H_
#define DESCENSION_GETTERMEMFUNCPTR_H_

#include "GetterFuncBase.h"

namespace Descension
{
    // T - Return type
    // U - Instance of the class
    // V - Member function pointer of the class
    template <typename T, typename U, typename V>
    class GetterMemFuncPtr: public GetterFuncBase<T>
    {
    protected:
        typedef V FuncPtr;
        FuncPtr func_;
        U* obj_;
    public:
        GetterMemFuncPtr(U* obj, V func)
            : func_(func), obj_(obj)
        {
        }

        T operator()()
        {
            return (obj_->*func_)();
        }
    };
}

#endif
