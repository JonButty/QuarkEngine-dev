/********************************************************************
	created:	2015/03/19
	filename: 	QELuaScript.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QELUASCRIPT_H
#define QELUASCRIPT_H

#include "QEDefines.h"
#include "QEScript.h"

#include <string>
#include <map>

struct QELuaScriptObject;

class QELuaScript : public QEScript
{
public:

    QE_API QELuaScript();
    QE_API ~QELuaScript();

public:

    QE_API int Load();
    QE_API int Unload();
    QE_API QEScriptObject* LoadScript(QE_IN_OPT QEScriptObject* scriptObj,
                                      QE_IN const std::string& filePath);
    QE_API QEScriptObject* RunScript(QE_IN_OPT QEScriptObject* scriptObj,
                                     QE_IN const std::string& filePath);
    QE_API virtual int ErrorCheck(QE_IN_OPT QEScriptObject* scriptObj);

public:
    
    template <typename T>
    QE_API T GetVariable(QE_IN QEScriptObject* scriptObj, 
                         QE_IN const std::string& varName )
    {

    }
    
private:

    QELuaScriptObject* load_(const std::string& filePath);

private:

    typedef std::string FilePath;
    typedef std::map<FilePath,QELuaScriptObject*> FilePathScriptObjectMap;

private:

    FilePathScriptObjectMap filePathScriptObjectMap_;

};

#endif