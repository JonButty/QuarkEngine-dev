/*!*****************************************************************************

\class 	QEScriptManager
\date 	2015/03/28
\author	Jonathan Butt
\brief	In charge of managing scripts regardless of the scripting language. The
        script manager holds a database of script objects which have been 
        requested to be loaded.
\todo create function to clear out unloaded scripts
\todo implement "reload" function
\todo add set<scriptobj*> for search by script object
\todo add map<string,scriptobj*> for search by filepath
\todo implement tests

*******************************************************************************/
#ifndef QESCRIPTMANAGER_H
#define QESCRIPTMANAGER_H

#include "QEModule.h"
#include "QEScriptObject.h"
#include "QEScript.h"

#include <string>
#include <map>

class QEScript;

class QEScriptManager : public QEModule//<QEScriptManager>
{
public:

    QEScriptManager();
    virtual ~QEScriptManager();

public:

    QE_INT Load();
    QE_INT Unload();
    QE_BOOL LoadScript(QE_IN_OUT QEScriptObject*& obj,
                              QE_IN const std::string& filePath);
    QE_BOOL LoadScript(QE_IN_OUT QEScriptObject*& scriptObj);
    void UnloadScript(QE_IN QEScriptObject*& scriptObj);
    QE_BOOL GetBool(QE_IN QEScriptObject*& scriptObj,
                           QE_IN const std::string& varName,
                           QE_OUT QE_BOOL* var);
    QE_BOOL GetInt(QE_IN QEScriptObject*& scriptObj,
                          QE_IN const std::string& varName,
                          QE_OUT QE_INT* var);
    QE_BOOL GetFloat(QE_IN QEScriptObject*& scriptObj,
                            QE_IN const std::string& varName,
                            QE_OUT QE_FLOAT* var);
    QE_BOOL GetString(QE_IN QEScriptObject*& scriptObj,
                             QE_IN const std::string& varName,
                             QE_OUT std::string* var);

private:

    int checkSupported_(const std::string& filePath) const;

    template <typename T>
    QE_BOOL getVariable_(QEScriptObject*& scriptObj,
                         const std::string& varName,
                         T* var,
                         QE_BOOL(QEScript::* a)(QEScriptObject*&,const std::string&,T*))
    {
        FilePathScriptObjectMap::iterator it = filePathScriptObjectMap_.find(scriptObj->filePath_);

        if(it != filePathScriptObjectMap_.end())
        {
            // Script exists
            if(scriptObj->status_ != QEScriptObject::S_OK)
            {
                // Script was not loaded properly
                scriptLangTable_[it->second.lang_]->ErrorCheck(scriptObj);
                return false;
            }

            // Get variable
            bool retVal = (scriptLangTable_[it->second.lang_]->*a)(scriptObj,varName,var);
            
            if(!retVal)
                scriptLangTable_[it->second.lang_]->ErrorCheck(scriptObj);
            return retVal;
        }

        // Script was not loaded
        //QE_LOGE(scriptObj->filePath_ << " was not loaded before.");
        return false;
    }

private:

    enum ScriptLang
    {
        SL_LUA,
        SL_TOTAL
    };

    struct ScriptObjectMeta
    {
        QE_INT lang_;
        QE_UINT refCount_;
        QEScriptObject* obj_;
    };

private:

    typedef std::string FilePath;
    typedef std::map<FilePath,ScriptObjectMeta> FilePathScriptObjectMap;

private:

    FilePathScriptObjectMap filePathScriptObjectMap_;
    static QEScript* scriptLangTable_[SL_TOTAL];

};

#endif
