/******************************************************************************/
/*!
\file		GetterFuncBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GETTERFUNCBASE_H_
#define DESCENSION_GETTERFUNCBASE_H_

namespace Descension
{
    template <typename T>
    class GetterFuncBase
    {
    public:
        typedef T ReturnType;
    public:
        virtual T operator()() = 0;
    };
}

#endif
