#include "stdafx.h"
#include "QEScriptManager.h"
#include "QELuaScript.h"

QEScript* QEScriptManager::scriptLangTable_[] =
{
    new QELuaScript()
};

/*!*****************************************************************************

\name   QEScriptManager

\brief

*******************************************************************************/
QEScriptManager::QEScriptManager()
{
}

/*!*****************************************************************************

\name   ~QEScriptManager

\brief

*******************************************************************************/
QEScriptManager::~QEScriptManager()
{
    for(QE_UINT i = 0; i < SL_TOTAL; ++i)
        delete scriptLangTable_[i];
}

/*!*****************************************************************************

\name   Load
\return QE_API QE_INT

\brief

*******************************************************************************/
QE_API QE_INT QEScriptManager::Load()
{
    QE_INT returnCode = 0;

    for(QE_UINT i = 0; i < SL_TOTAL; ++i)
    {
        returnCode = (returnCode == 0) ? scriptLangTable_[i]->Load() : returnCode;
    }
    return returnCode;
}

/*!*****************************************************************************

\name   Unload
\return QE_API QE_INT

\brief

*******************************************************************************/
QE_API QE_INT QEScriptManager::Unload()
{
    // Unload all script objects first
    FilePathScriptObjectMap::iterator it = filePathScriptObjectMap_.begin();
    
    while(it != filePathScriptObjectMap_.end())
    {
        scriptLangTable_[it->second.lang_]->UnloadScript(it->second.obj_);
        ++it;
    }

    QE_INT returnCode = 0;

    for(QE_UINT i = 0; i < SL_TOTAL; ++i)
    {
        returnCode = (returnCode == 0) ? scriptLangTable_[i]->Unload() : returnCode;
    }
    return returnCode;
}

/*!*****************************************************************************

\name   LoadScript
\param	QE_IN_OUT QEScriptObject *& obj
\param	QE_IN const std::string & filePath
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QEScriptManager::LoadScript(QE_IN_OUT QEScriptObject*& obj,
                                           QE_IN const std::string& filePath)
{
    QE_LOGV("Loading script " << filePath);

    QE_INT index = checkSupported_(filePath);
    
    if(index < 0)
    {
        QE_LOGW("Script not supported: " << filePath);
        return false;
    }

    // Check if script has been loaded
    FilePathScriptObjectMap::iterator it = filePathScriptObjectMap_.find(filePath);

    if(it != filePathScriptObjectMap_.end())
    {
        ++it->second.refCount_;
        scriptLangTable_[index]->LoadScript(it->second.obj_);
        obj = it->second.obj_;
        return scriptLangTable_[index]->ErrorCheck(it->second.obj_);
    }
    else
    {
        QEScriptObject* scriptObj = scriptLangTable_[index]->LoadScript(filePath);
        ScriptObjectMeta meta = {index,1,scriptObj};
        filePathScriptObjectMap_.insert(std::pair<FilePath,ScriptObjectMeta>(filePath,meta));
        obj = scriptObj;
        return scriptLangTable_[index]->ErrorCheck(scriptObj);
    }
}

/*!*****************************************************************************

\name   LoadScript
\param	QE_IN QEScriptObject * scriptObj
\return QE_API QEScriptObject*

\brief

*******************************************************************************/
QE_API QE_BOOL QEScriptManager::LoadScript(QE_IN_OUT QEScriptObject*& scriptObj)
{
    QE_INT index = checkSupported_(scriptObj->filePath_);
    if(index < 0)
    {
        QE_LOGW("Script not supported: " << scriptObj->filePath_);
        return false;
    }

    // Check if script has been loaded
    FilePathScriptObjectMap::iterator it = filePathScriptObjectMap_.find(scriptObj->filePath_);

    if(it != filePathScriptObjectMap_.end())
    {
        ++it->second.refCount_;
        scriptLangTable_[index]->LoadScript(it->second.obj_);
        scriptLangTable_[index]->ErrorCheck(it->second.obj_);
        scriptObj = it->second.obj_;
        return true;
    }
    else
    {
        QE_LOGW("Invalid object");
        return false;
    }
}

/*!*****************************************************************************

\name   UnloadScript
\param	QE_IN QEScriptObject * & scriptObj
\return QE_API void

\brief

*******************************************************************************/
QE_API void QEScriptManager::UnloadScript(QE_IN QEScriptObject*& scriptObj)
{
    QE_LOGV("Unloading script " << scriptObj->filePath_);

    QE_INT index = checkSupported_(scriptObj->filePath_);

    if(index < 0)
        QE_LOGW("Script not supported: " << scriptObj->filePath_);

    // Check if script has been loaded
    FilePathScriptObjectMap::iterator it = filePathScriptObjectMap_.find(scriptObj->filePath_);

    if(it != filePathScriptObjectMap_.end())
    {
        // Only unload if this is the last reference
        if(it->second.refCount_ == 1)
        {
            scriptLangTable_[index]->UnloadScript(scriptObj);
            filePathScriptObjectMap_.erase(it);
        }
        else
            --it->second.refCount_;
    }
    else
        QE_LOGW("Invalid object");
}

/*!*****************************************************************************

\name   GetBool
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT QE_BOOL * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QEScriptManager::GetBool(QE_IN QEScriptObject*& scriptObj,
                                        QE_IN const std::string& varName,
                                        QE_OUT QE_BOOL* var)
{
    return getVariable_(scriptObj,varName,var,&QEScript::GetBool);
}

/*!*****************************************************************************

\name   GetInt
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT QE_INT * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QEScriptManager::GetInt(QE_IN QEScriptObject*& scriptObj,
                                       QE_IN const std::string& varName,
                                       QE_OUT QE_INT* var)
{
    return getVariable_(scriptObj,varName,var,&QEScript::GetInt);
}

/*!*****************************************************************************

\name   GetFloat
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT QE_FLOAT * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QEScriptManager::GetFloat(QE_IN QEScriptObject*& scriptObj,
                                         QE_IN const std::string& varName,
                                         QE_OUT QE_FLOAT* var)
{
    return getVariable_(scriptObj,varName,var,&QEScript::GetFloat);
}

/*!*****************************************************************************

\name   GetString
\param	QE_IN QEScriptObject * & scriptObj
\param	QE_IN const std::string & varName
\param	QE_OUT std::string * var
\return QE_API QE_BOOL

\brief

*******************************************************************************/
QE_API QE_BOOL QEScriptManager::GetString(QE_IN QEScriptObject*& scriptObj,
                                          QE_IN const std::string& varName,
                                          QE_OUT std::string* var)
{
    return getVariable_(scriptObj,varName,var,&QEScript::GetString);
}

/*!*****************************************************************************

\name   checkSupported_
\param	const std::string & filePath
\return bool

\brief

*******************************************************************************/
int QEScriptManager::checkSupported_(const std::string& filePath) const
{
    std::string extension(filePath.substr(filePath.find_last_of('.') + 1));
    
    for(QE_UINT i = 0; i < SL_TOTAL; ++i)
    {
        if(scriptLangTable_[i]->GetExtension() == extension)
            return i;
    }
    return -1;
}