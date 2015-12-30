#include "stdafx.h"
#include "QEScript.h"

/*!*****************************************************************************

\name   QEScript
\param	QE_IN const std::string & ext
\return QE_API

\brief

*******************************************************************************/
QEScript::QEScript(QE_IN const std::string& ext)
    :   ext_(ext)
{
}

/*!*****************************************************************************

\name   ~QEScript
\brief

*******************************************************************************/
QEScript::~QEScript()
{
}

/*!*****************************************************************************

\name   GetExtension
\return const std::string&

\brief

*******************************************************************************/
const std::string& QEScript::GetExtension() const
{
    return ext_;
}

/*!*****************************************************************************

\name   Load
\return QE_INT

\brief

*******************************************************************************/
QE_INT QEScript::Load()
{
    return 0;
}

/*!*****************************************************************************

\name   Unload
\return QE_INT

\brief

*******************************************************************************/
QE_INT QEScript::Unload()
{
    return 0;
}

/*!*****************************************************************************

\name   RunScript
\param	QE_IN_OPT QEScriptObject * scriptObj
\param	QE_IN const std::string & filePath
\return QEScriptObject*

\brief

*******************************************************************************/
QEScriptObject* QEScript::RunScript(QE_IN_OPT QEScriptObject* scriptObj,
                                    QE_IN const std::string& filePath)
{
    return 0;
}