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
    static QEModule* __QEMODULES [] = {

#define QE_DECLARE_MODULE(module)\
    module::InstancePtr(),

#define QE_MODULES_END 0 };

#define QE_MODULE_PTR(module) reinterpret_cast<module*>(module::InstancePtr())

#endif