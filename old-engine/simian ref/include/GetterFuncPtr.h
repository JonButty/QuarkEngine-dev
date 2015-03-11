/******************************************************************************/
/*!
\file		GetterFuncPtr.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GETTERFUNCPTR_H_
#define DESCENSION_GETTERFUNCPTR_H_

#include "GetterFuncBase.h"

namespace Descension
{
    template <typename T>
    class GetterFuncPtr: public GetterFuncBase<T>
    {
    protected:
        typedef T(*FuncPtr)();
        FuncPtr func_;
    public:
        GetterFuncPtr(FuncPtr func)
            : func_(func)
        {
        }

        T operator()()
        {
            return func_();
        }
    };
}

#endif
