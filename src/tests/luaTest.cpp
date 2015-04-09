#include "stdafx.h"
#include "luaTest.h"

void LuaTest::Test()
{
    QEScriptObject* scriptObj = QEScriptManager::InstancePtr()->LoadScript("data/config/config.lua");
    QE_BOOL boolVal = true;
    QEScriptManager::InstancePtr()->GetBool(scriptObj,"testString",&boolVal);
    QE_LOGV("TestBool is " << boolVal);

    /*lua_State* L = luaL_newstate();
    int i = luaL_dofile(L, "data/config/config.lua");
    luaL_openlibs(L);
    lua_pcall(L, 0, 0, 0);
    LuaRef s = getGlobal(L, "testString");
    LuaRef n = getGlobal(L, "number");
    std::string luaString = s.cast<std::string>();
    int answer = n.cast<int>();
    std::cout << luaString << std::endl;
    std::cout << "And here's our number:" << answer << std::endl;
    */
}