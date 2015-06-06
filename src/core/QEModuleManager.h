/*!*****************************************************************************

\class 	QEModuleManager
\date 	2015/05/22
\author	Jonathan Butt
\brief	
\todo change coding convention for private vars to start with '_'
*******************************************************************************/

#ifndef QEMODULEMANAGER_H
#define QEMODULEMANAGER_H

#include "common/QETypes.h"
#include "common/QEDefines.h"
#include "util/QESingleton.h"

#include <unordered_map>

class QEModule;

class QEModuleManager : public QESingleton<QEModuleManager>
{
public:

    QEModuleManager();
    ~QEModuleManager();

public:

    void Load();
    void Initialize();
    void Update();
    void Deinitialize();
    void Unload();

public:

    template <typename T>
    std::pair<bool,size_t> RegisterModule(QE_IN T module)
    {
        size_t hashcode = typeid(*module).hash_code();
        return std::pair<bool,size_t>(_moduleTable.insert(std::pair<size_t,QEModule*>(hashcode,module)).second,hashcode);
    }

    template <typename T>
    T* GetModule(T module)
    {
        size_t hashcode = typeid(module).hash_code();
        ModuleTable::iterator it = _moduleTable.find(hashcode);
        
        if(it == _moduleTable.end())
            return 0;
        return reinterpret_cast<T*>(it->second);
    }

private:

    typedef std::unordered_map<size_t,QEModule*> ModuleTable;
    
private:
    
    ModuleTable _moduleTable;
};

#endif