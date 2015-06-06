#ifndef QEDEFINES_H
#define QEDEFINES_H

#define QE_API

// Function Annotations
#define QE_IN
#define QE_IN_OPT
#define QE_OUT
#define QE_IN_OUT

#define QE_UNUSED(arg) arg;


#define QE_MODULES_BEGIN \
    typedef std::pair<bool,size_t> __MODULE_INFO;\
    static __MODULE_INFO __QEMODULES [] = { __MODULE_INFO{false,0}

#define QE_DECLARE_MODULE(module)\
    , QEModuleManager::InstancePtr()->RegisterModule(new module())

#define QE_MODULES_END };

#define  QE_GET_MODULE(module) QEModuleManager::InstancePtr()->GetModule(module())
#endif