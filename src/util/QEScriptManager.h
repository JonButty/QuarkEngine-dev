/*!*****************************************************************************

\class 	QEScriptManager
\date 	2015/03/28
\author	Jonathan Butt
\brief	In charge of managing scripts regardless of the scripting language. The
        script manager holds a database of script objects which have been 
        requested to be loaded.
\todo create function to clear out unloaded scripts

*******************************************************************************/
#ifndef QESCRIPTMANAGER_H
#define QESCRIPTMANAGER_H

#include "QEManager.h"
#include "QEManager.h"
#include "QEScriptObject.h"
#include "QEScript.h"

#include <string>
#include <map>

class QEScript;

class QEScriptManager : public QEManager<QEScriptManager>
{
public:

    QE_API QEScriptManager();
    QE_API virtual ~QEScriptManager();

public:

    QE_API QE_INT Load();
    QE_API QE_INT Unload();
    QE_API QEScriptObject* LoadScript(QE_IN const std::string& filePath);
    QE_API QEScriptObject* LoadScript(QE_IN QEScriptObject*& scriptObj);
    QE_API QE_BOOL GetBool(QE_IN QEScriptObject*& scriptObj,
                           QE_IN const std::string& varName,
                           QE_OUT QE_BOOL* var);
    QE_API QE_BOOL GetInt(QE_IN QEScriptObject*& scriptObj,
                          QE_IN const std::string& varName,
                          QE_OUT QE_INT* var);
    QE_API QE_BOOL GetFloat(QE_IN QEScriptObject*& scriptObj,
                            QE_IN const std::string& varName,
                            QE_OUT QE_FLOAT* var);
    QE_API QE_BOOL GetString(QE_IN QEScriptObject*& scriptObj,
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
                QE_LOGE(scriptObj->filePath_ << " was loaded with errors, please reload script");
                return false;
            }

            // Get variable
            return (scriptLangTable_[it->second.lang_]->*a)(scriptObj,varName,var);
        }

        // Script was not loaded
        QE_LOGE(scriptObj->filePath_ << " was not loaded before.");
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
