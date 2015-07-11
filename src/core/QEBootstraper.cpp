#include "stdafx.h"
#include "core/QEBootstraper.h"

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
	return true;
}

/*!*****************************************************************************

\name   _loadModules
\return void

\brief

*******************************************************************************/
void QEBootstrapper::_loadModules()
{
	QE_UINT i = 0;
	while(__QEMODULES[i])
	{
		__QEMODULES[i]->Load();
	}
	
	i = 0;
	while(__QEMODULES[i])
	{
		__QEMODULES[i]->Initialize();
	}
}

/*!*****************************************************************************

\name   _unloadModules
\return void

\brief

*******************************************************************************/
void QEBootstrapper::_unloadModules()
{
	QE_UINT i = 0;
	while(__QEMODULES[i])
	{
		__QEMODULES[i]->Deinitialize();
	}

	i = 0;
	while(__QEMODULES[i])
	{
		__QEMODULES[i]->Unload();
	}
}