#include "stdafx.h"
#include "QELuaScript.h"
#include "QELuaScriptObject.h"

#include <LuaBridge.h>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

using namespace luabridge;

QELuaScript::QELuaScript()
{
}

QELuaScript::~QELuaScript()
{
}

int QELuaScript::Load()
{
    return 0;
}

int QELuaScript::Unload()
{
    return 0;
}

QE_API QEScriptObject* QELuaScript::LoadScript(QEScriptObject* scriptObj,
                                               const std::string& filePath)
{
    if(filePath.size() == 0)
        return 0;
    
    FilePathScriptObjectMap::const_iterator it = filePathScriptObjectMap_.find(filePath);
    
    // Script has been loaded
    if(it != filePathScriptObjectMap_.end())
    {
        // If script was not loaded previously try again
        return it->second;
    }
    // This is a new script
    else
    {

    }
}

QEScriptObject* QELuaScript::RunScript(QEScriptObject* scriptObj,
                                      const std::string& filePath)
{
    return 0;
}

int QELuaScript::ErrorCheck(QEScriptObject* scriptObj)
{
    return 0;
}

//------------------------------------------------------------------------------

QELuaScriptObject* QELuaScript::load_(const std::string& filePath)
{
    lua_State* l = luaL_newstate();
    int status = luaL_loadfile(l,filePath.c_str());

    if(status)
    {
        // Cannot load script
        //fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
    }

    luaL_openlibs(l);
    return 0;
}