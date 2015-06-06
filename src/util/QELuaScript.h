/*!*****************************************************************************

\class 	QELuaScript
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QELUASCRIPT_H
#define QELUASCRIPT_H

#include "common\QEDefines.h"
#include "QEScript.h"

#include <string>
#include <map>

struct QEScriptObject;

class QELuaScript : public QEScript
{

public:

    QELuaScript();

public:

    QEScriptObject* LoadScript(QE_IN QEScriptObject*& scriptObj);
    QEScriptObject* LoadScript(QE_IN const std::string& filePath);
    void UnloadScript(QE_IN QEScriptObject*& scriptObj);
    QEScriptObject* RunScript(QE_IN_OPT QEScriptObject* scriptObj,
                                     QE_IN const std::string& filePath);
    QE_BOOL ErrorCheck(QE_IN QEScriptObject*& scriptObj);
    QE_BOOL GetBool(QE_IN QEScriptObject*& scriptObj,
                           QE_IN const std::string& varName,
                           QE_OUT QE_BOOL* var) ;
    QE_BOOL GetInt(QE_IN QEScriptObject*& scriptObj,
                          QE_IN const std::string& varName,
                          QE_OUT QE_INT* var);
    QE_BOOL GetFloat(QE_IN QEScriptObject*& scriptObj,
                            QE_IN const std::string& varName,
                            QE_OUT QE_FLOAT* var);
    QE_BOOL GetString(QE_IN QEScriptObject*& scriptObj,
                             QE_IN const std::string& varName,
                             QE_OUT std::string* var);

public:
    
    template <typename T>
    T GetVariable(QE_IN QEScriptObject*& scriptObj, 
                         QE_IN const std::string& varName )
    {
        LuaRef lRef = getGlobal(reinterpret_cast<QELuaScriptObject*&>(scriptObj)->luaState_,
                                varName.c_str());
        return lRef.cast<T>();
    }
};

#endif