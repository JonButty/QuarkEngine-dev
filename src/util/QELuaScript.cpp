#include "stdafx.h"
#include "util/QELuaScript.h"
#include "util/QELuaScriptObject.h"
#include "util/QELog.h"

#include <LuaBridge.h>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

using namespace luabridge;

/*!*****************************************************************************

\name   QELuaScript
\return QE_API

\brief

*******************************************************************************/
QE_API QELuaScript::QELuaScript()
    :   QEScript("lua")
{
}

/*!*****************************************************************************

\name   LoadScript
\param	QE_IN QEScriptObject * scriptObj
\return QE_API QEScriptObject*

\brief

*******************************************************************************/
QE_API QEScriptObject* QELuaScript::LoadScript(QE_IN QEScriptObject*& scriptObj)
{
    if(!scriptObj)
        return 0;

    QE_LOGV("Loading script " << scriptObj->filePath_);

    QELuaScriptObject*& luaScriptObj = reinterpret_cast<QELuaScriptObject*&>(scriptObj);
    lua_State* luaStateRef(luaScriptObj->luaState_);

    int code = luaL_dofile(luaStateRef,scriptObj->filePath_.c_str());

    if(code)
    {
        QE_LOGW("Failed to open script: ");
        QE_LOGW(lua_tostring(luaStateRef,-1));
        luaScriptObj->status_ = QEScriptObject::S_FILE_OPEN_ERROR;
        return luaScriptObj;
    }

    luaL_openlibs(luaStateRef);

    code = lua_pcall(luaStateRef,0,0,0);

    if(code)
    {
        QE_LOGW("Failed to load script: ");
        QE_LOGE(lua_tostring(luaStateRef,-1));
        luaScriptObj->status_ = QEScriptObject::S_COMPILE_ERRORS;
        return luaScriptObj;
    }
    luaScriptObj->status_ = QEScriptObject::S_OK;
    return luaScriptObj;
}

/*!*****************************************************************************

\name   LoadScript
\param	QE_IN const std::string & filePath
\return QE_API QEScriptObject*

\brief

*******************************************************************************/
QE_API QEScriptObject* QELuaScript::LoadScript(QE_IN const std::string& filePath)
{
    QELuaScriptObject* luaScriptObj = new QELuaScriptObject();
    luaScriptObj->luaState_ = luaL_newstate();
    luaScriptObj->filePath_ = filePath;
    int code = luaL_loadfile(luaScriptObj->luaState_,filePath.c_str());

    if(code)
    {
        luaScriptObj->status_ = QEScriptObject::S_FILE_OPEN_ERROR;
        return luaScriptObj;
    }

    luaL_openlibs(luaScriptObj->luaState_);

    code = lua_pcall(luaScriptObj->luaState_,0,0,0);

    if(code)
    {
        luaScriptObj->status_ = QEScriptObject::S_COMPILE_ERRORS;
        return luaScriptObj;
    }
    luaScriptObj->status_ = QEScriptObject::S_OK;
    return luaScriptObj;
}

/*!*****************************************************************************

\name   UnloadScript
\param	QE_IN QEScriptObject * scriptObj
\return QE_API void

\brief

*******************************************************************************/
QE_API void QELuaScript::UnloadScript(QE_IN QEScriptObject*& scriptObj)
{
    QELuaScriptObject*& luaScriptObj = reinterpret_cast<QELuaScriptObject*&>(scriptObj);
    lua_close(luaScriptObj->luaState_);
    delete scriptObj;
    scriptObj = 0;
}

/*!*****************************************************************************

\name   RunScript
\param	QE_IN_OPT QEScriptObject * scriptObj
\param	QE_IN const std::string & filePath
\return QE_API QEScriptObject*

\brief

*******************************************************************************/
QE_API QEScriptObject* QELuaScript::RunScript(QE_IN_OPT QEScriptObject* scriptObj,
                                              QE_IN const std::string& filePath)
{
    return 0;
}

/*!*****************************************************************************

\name   ErrorCheck
\param	QE_IN QEScriptObject * scriptObj
\return QE_API QE_INT

\brief

*******************************************************************************/
QE_API QE_BOOL QELuaScript::ErrorCheck(QE_IN QEScriptObject*& scriptObj)
{
    switch(scriptObj->status_)
    {
    case QEScriptObject::S_OK:
        return true;
    case QEScriptObject::S_FILE_OPEN_ERROR:
        QE_LOGW("Failed to open script: " << lua_tostring(reinterpret_cast<QELuaScriptObject*>(scriptObj)->luaState_,-1));
        break;
    case QEScriptObject::S_COMPILE_ERRORS:
        QE_LOGW("Failed to load script: " << lua_tostring(reinterpret_cast<QELuaScriptObject*>(scriptObj)->luaState_,-1));
        break;
    case QEScriptObject::S_VAR_NOT_FOUND:
        QE_LOGW("Failed to find previous variable");
        break;
    case QEScriptObject::S_INVALID_VAR_TYPE:
        QE_LOGW("Invalid type for previous variable");
        break;
    }
    return false;
}

/*!*****************************************************************************

\name   GetBool
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT QE_BOOL * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QELuaScript::GetBool(QE_IN QEScriptObject*& scriptObj,
                                    QE_IN const std::string& varName,
                                    QE_OUT QE_BOOL* var)
{
    LuaRef lRef = getGlobal(reinterpret_cast<QELuaScriptObject*&>(scriptObj)->luaState_,
                            varName.c_str());

    if(lRef.isNil())
    {
        scriptObj->status_ = QEScriptObject::S_VAR_NOT_FOUND;
        return false;
    }

    if(lRef.isFunction() || lRef.isLightUserdata() || lRef.isNumber() ||
       lRef.isString() || lRef.isTable() || lRef.isThread() || lRef.isUserdata())
    {
        scriptObj->status_ = QEScriptObject::S_INVALID_VAR_TYPE;
        return false;
    }

    *var = lRef.cast<QE_BOOL>();
    return true;
}

/*!*****************************************************************************

\name   GetInt
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT QE_INT * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QELuaScript::GetInt(QE_IN QEScriptObject*& scriptObj,
                                   QE_IN const std::string& varName,
                                   QE_OUT QE_INT* var)
{
    LuaRef lRef = getGlobal(reinterpret_cast<QELuaScriptObject*&>(scriptObj)->luaState_,
                            varName.c_str());

    if(lRef.isNil())
    {
        scriptObj->status_ = QEScriptObject::S_VAR_NOT_FOUND;
        return false;
    }

    if(lRef.isFunction() || lRef.isLightUserdata() || lRef.isString() ||
       lRef.isTable() || lRef.isThread() || lRef.isUserdata())
    {
        scriptObj->status_ = QEScriptObject::S_INVALID_VAR_TYPE;
        return false;
    }

    *var = lRef.cast<QE_INT>();
    return true;
}

/*!*****************************************************************************

\name   GetFloat
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT QE_FLOAT * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QELuaScript::GetFloat(QE_IN QEScriptObject*& scriptObj,
                                     QE_IN const std::string& varName,
                                     QE_OUT QE_FLOAT* var)
{
    LuaRef lRef = getGlobal(reinterpret_cast<QELuaScriptObject*&>(scriptObj)->luaState_,
                            varName.c_str());

    if(lRef.isNil())
    {
        scriptObj->status_ = QEScriptObject::S_VAR_NOT_FOUND;
        return false;
    }

    if(lRef.isFunction() || lRef.isLightUserdata() || lRef.isString() ||
       lRef.isTable() || lRef.isThread() || lRef.isUserdata())
    {
        scriptObj->status_ = QEScriptObject::S_INVALID_VAR_TYPE;
        return false;
    }

    *var = lRef.cast<QE_FLOAT>();
    return true;
}

/*!*****************************************************************************

\name   GetString
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT std::string * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QELuaScript::GetString(QE_IN QEScriptObject*& scriptObj,
                                      QE_IN const std::string& varName,
                                      QE_OUT std::string* var)
{
    LuaRef lRef = getGlobal(reinterpret_cast<QELuaScriptObject*&>(scriptObj)->luaState_,
                            varName.c_str());

    if(lRef.isNil())
    {
        scriptObj->status_ = QEScriptObject::S_VAR_NOT_FOUND;
        return false;
    }

    if(!lRef.isString())
    {
        scriptObj->status_ = QEScriptObject::S_VAR_NOT_FOUND;
        return false;
    }
    *var = lRef.cast<std::string>();
    return true;
}