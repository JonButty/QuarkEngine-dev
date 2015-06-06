/*!*****************************************************************************

\class 	QEScript
\date 	2015/03/28
\author	Jonathan Butt
\brief	The base class for any scripting language. The scripts should be
        responsible for the details of how to load/unload/run a script based on
        the language specifications. The QEScriptManager will be in charge of 
        instantiating a copy of each derived class of QEScript to be able to 
        support script functionality.
        
*******************************************************************************/
#ifndef QESCRIPT_H
#define QESCRIPT_H

#include "common/QEDefines.h"

#include <string>
#include <assert.h>

struct QEScriptObject;

class QEScript
{
public:

    QEScript(QE_IN const std::string& ext);
    virtual ~QEScript();

public:

    const std::string& GetExtension() const;

public:

    virtual QE_INT Load();
    virtual QE_INT Unload();
    virtual QEScriptObject* LoadScript(QE_IN QEScriptObject*& scriptObj) = 0;
    virtual QEScriptObject* LoadScript(QE_IN const std::string& filePath) = 0;
    virtual void UnloadScript(QE_IN QEScriptObject*& scriptObj) = 0;
    virtual QEScriptObject* RunScript(QE_IN_OPT QEScriptObject* scriptObj, 
                                             QE_IN const std::string& filePath);
    virtual QE_BOOL ErrorCheck(QE_IN_OPT QEScriptObject*& scriptObj) = 0;
    virtual QE_BOOL GetBool(QE_IN QEScriptObject*& scriptObj,
                                   QE_IN const std::string& varName,
                                   QE_OUT QE_BOOL* var) = 0;
    virtual QE_BOOL GetInt(QE_IN QEScriptObject*& scriptObj,
                                  QE_IN const std::string& varName,
                                  QE_OUT QE_INT* var) = 0;
    virtual QE_BOOL GetFloat(QE_IN QEScriptObject*& scriptObj,
                                    QE_IN const std::string& varName,
                                    QE_OUT QE_FLOAT* var) = 0;
    virtual QE_BOOL GetString(QE_IN QEScriptObject*& scriptObj,
                                     QE_IN const std::string& varName,
                                     QE_OUT std::string* var) = 0;
    //virtual QE_DOUBLE GetTable(QE_IN QEScriptObject*& scriptObj,
    //                                  QE_IN const std::string& varName) = 0;
private:

    const std::string ext_;
};

#endif