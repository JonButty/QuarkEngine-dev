/*!*****************************************************************************

\file 	QELogManager.h
\date 	2015/03/26
\author	Jonathan Butt
\brief	
\todo Filter by output level

*******************************************************************************/
#ifndef QELOGMANAGER_H
#define QELOGMANAGER_H

#include "common/QEDefines.h"
#include "common/QETypes.h"
#include "QEManager.h"

#include <string>
#include <vector>
#include <set>
#include <string>

class QELogger;

class QELogManager : public QEManager<QELogManager>
{
public:

    enum Level
    {
        L_VERBOSE,
        L_WARNING,
        L_ERROR,
        L_TOTAL
    };

public:

    QE_API QELogManager();
    QE_API ~QELogManager();

public:

    QE_API QE_INT Load();
    QE_API QE_INT Unload();
    QE_API void Log(QE_IN const std::string& filePath,
                    QE_IN QE_UINT lineNumber,
                    QE_IN const std::string& msg,
                    QE_IN QE_UINT level);
    QE_API void SetFilter(QE_IN_OPT const std::vector<std::string>* filterList);
    
private:

    std::string filterMessage_(const std::string& file,
                               QE_UINT lineNumber,
                               const std::string& msg,
                               QE_UINT level);
    bool isFileFiltered_(const std::string& file);

private:

    typedef std::string File;
    typedef std::set<File> FilterList;
    typedef std::vector<QELogger*> LoggerList;

private:

    FilterList filterList_;
    LoggerList loggerList_;
};

#endif