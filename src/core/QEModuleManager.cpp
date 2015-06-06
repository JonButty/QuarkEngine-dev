#include "stdafx.h"

/*!*****************************************************************************

\name   QEModuleManager
\return

\brief

*******************************************************************************/
QEModuleManager::QEModuleManager()
{
}

/*!*****************************************************************************

\name   ~QEModuleManager
\return

\brief

*******************************************************************************/
QEModuleManager::~QEModuleManager()
{
}

/*!*****************************************************************************

\name   Load
\return void

\brief

*******************************************************************************/
void QEModuleManager::Load()
{
    ModuleTable::iterator it = _moduleTable.begin();
    
    while (it != _moduleTable.end())
    {
        it->second->Load();
        ++it;
    }
}

/*!*****************************************************************************

\name   Initialize
\return void

\brief

*******************************************************************************/
void QEModuleManager::Initialize()
{

}

/*!*****************************************************************************

\name   Update
\return void

\brief

*******************************************************************************/
void QEModuleManager::Update()
{

}

/*!*****************************************************************************

\name   Deinitialize
\return void

\brief

*******************************************************************************/
void QEModuleManager::Deinitialize()
{

}

/*!*****************************************************************************

\name   Unload
\return void

\brief

*******************************************************************************/
void QEModuleManager::Unload()
{

}
