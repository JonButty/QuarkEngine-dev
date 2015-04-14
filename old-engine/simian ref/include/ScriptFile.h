/*************************************************************************/
/*!
\file		ScriptFile.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SCRIPTFILE_H_
#define DESCENSION_SCRIPTFILE_H_

#include "Simian/Delegate.h"
#include "Simian/DataLocation.h"
#include "Simian/LogManager.h"

#include "squirrel.h"

#include <cstdarg>
#include <cstdlib>

namespace Descension
{
    template <class T>
    class ScriptFile
    {
    public:
        struct CallParameter: public Simian::DelegateParameter
        {
            HSQUIRRELVM VM;
            bool ReturnValue;
        };
    private:
        HSQUIRRELVM vm_;
    public:
        HSQUIRRELVM VM() const { return vm_; }
    private:
        template <void (T::*callback)(Simian::DelegateParameter&)>
        static SQInteger call_(HSQUIRRELVM vm)
        {
            // set up the param for sending to delegated function
            CallParameter param;
            param.VM = vm;
            param.ReturnValue = false;

            // get callee from the roottable
            sq_pushroottable(vm);
            sq_pushstring(vm, "callee", -1);
            sq_get(vm, -2);
            T* callee;
            sq_getuserpointer(vm, -1, (SQUserPointer*)&callee);
            sq_pop(vm, 2); // pop the user pointer and the roottable

            // perform delegate
            (callee->*callback)(param);

            // return the return value
            return param.ReturnValue ? 1 : 0;
        }

        static void print_(HSQUIRRELVM, const SQChar* formatString, ...)
        {
            va_list args;
            va_start(args, formatString);
            char str[512];
            vsprintf(str, formatString, args);
            va_end(args);
            SVerbose(str);
        }

        static void error_(HSQUIRRELVM, const SQChar* formatString, ...)
        {
            va_list args;
            va_start(args, formatString);
            char str[512];
            vsprintf(str, formatString, args);
            va_end(args);
            SWarn(str);
        }
    public:
        ScriptFile()
            : vm_(sq_open(1024))
        {
            sq_setprintfunc(vm_, print_, error_);
        }

        ~ScriptFile()
        {
            sq_close(vm_);
        }

        bool Load(T* callee, const Simian::DataLocation& data)
        {
            sq_compilebuffer(vm_, data.Memory(), data.Size(), 
                data.Identifier().c_str(), true);

            sq_pushroottable(vm_);
            sq_pushstring(vm_, "callee", -1);
            sq_pushuserpointer(vm_, (SQUserPointer)callee);
            sq_newslot(vm_, -3, false);

            if (SQ_FAILED(sq_call(vm_, 1, false, true)))
            {
                SWarn("Script loading failed: \"" << data.Identifier() << "\".");
                return false;
            }

            return true;
        }

        void Unload()
        {
            sq_settop(vm_, 0);
        }

        bool Do(T* callee, const Simian::DataLocation& data)
        {
            SQInteger top = sq_gettop(vm_);

            if (!Load(callee, data))
                return false;

            sq_settop(vm_, top);

            return true;
        }

        template <void (T::*callback)(Simian::DelegateParameter&)>
        void RegisterFunction(const std::string& functionName)
        {
            // add an entry for the function unto the root table
            sq_pushroottable(vm_);
            sq_pushstring(vm_, functionName.c_str(), -1);
            sq_newclosure(vm_, &call_<callback>, 0);
            // pops the last 2 elements and creates the entry
            sq_newslot(vm_, -3, false);
            // pop the roottable
            sq_pop(vm_, 1);
        }
    };
}

#endif
