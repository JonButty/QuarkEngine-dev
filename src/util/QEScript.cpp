#include "stdafx.h"
#include "QEScript.h"

/*!*****************************************************************************

\name   QEScript
\param	QE_IN const std::string & ext
\return QE_API

\brief

*******************************************************************************/
QE_API QEScript::QEScript(QE_IN const std::string& ext)
    :   ext_(ext)
{
}

/*!*****************************************************************************

\name   ~QEScript
\brief

*******************************************************************************/
QE_API QEScript::~QEScript()
{
}

/*!*****************************************************************************

\name   GetExtension
\return QE_API const std::string&

\brief

*******************************************************************************/
QE_API const std::string& QEScript::GetExtension() const
{
    return ext_;
}

/*!*****************************************************************************

\name   Load
\return QE_INT

\brief

*******************************************************************************/
QE_API QE_INT QEScript::Load()
{
    return 0;
}

/*!*****************************************************************************

\name   Unload
\return QE_INT

\brief

*******************************************************************************/
QE_API QE_INT QEScript::Unload()
{
    return 0;
}

/*!*****************************************************************************

\name   RunScript
\param	QE_IN_OPT QEScriptObject * scriptObj
\param	QE_IN const std::string & filePath
\return QE_API QEScriptObject*

\brief

*******************************************************************************/
QE_API QEScriptObject* QEScript::RunScript(QE_IN_OPT QEScriptObject* scriptObj,
                                           QE_IN const std::string& filePath)
{
    return 0;
}