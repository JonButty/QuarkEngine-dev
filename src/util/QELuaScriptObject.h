/********************************************************************
	created:	2015/03/19
	filename: 	QELuaScriptObject.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QELUASCRIPTOBJECT_H
#define QELUASCRIPTOBJECT_H

#include "QEScriptObject.h"

extern "C"
{
    #include "lua.h"
}

struct QELuaScriptObject : public QEScriptObject
{
    lua_State* luaState_;
};

#endif
