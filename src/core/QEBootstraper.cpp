#include "core/QEBootstraper.h"
#include "core/QEModuleManager.h"

/*!*****************************************************************************

\name   QEBootstrapper
\return

\brief

*******************************************************************************/
QEBootstrapper::QEBootstrapper()
{

}

/*!*****************************************************************************

\name   ~QEBootstrapper
\return

\brief

*******************************************************************************/
QEBootstrapper::~QEBootstrapper()
{

}

/*!*****************************************************************************

\name   Load
\param	QE_IN QE_INT argc
\param	QE_IN QE_CHAR * * argv
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL QEBootstrapper::Load(QE_IN QE_INT argc,QE_IN QE_CHAR ** argv)
{
    QEModuleManager::InstancePtr()->Load();
}
