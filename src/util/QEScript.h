/*!*****************************************************************************

\class 	QEScript
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QESCRIPT_H
#define QESCRIPT_H

#include "common/QEDefines.h"

#include <string>

struct QEScriptObject;

class QEScript
{
public:

    QE_API QEScript();
    QE_API virtual ~QEScript();

public:

    QE_API virtual int Load();
    QE_API virtual int Unload();
    QE_API virtual QEScriptObject* LoadScript(QE_IN_OPT QEScriptObject* scriptObj,
                                              QE_IN const std::string& filePath);
    QE_API virtual QEScriptObject* RunScript(QE_IN_OPT QEScriptObject* scriptObj, 
                                             QE_IN const std::string& filePath);
    QE_API virtual int ErrorCheck(QE_IN_OPT QEScriptObject* scriptObj);

};

#endif