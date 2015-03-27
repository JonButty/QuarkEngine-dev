/*!*****************************************************************************

\file 	QELogManager.h
\date 	2015/03/26
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QELOGMANAGER_H
#define QELOGMANAGER_H

#include "common/QEDefines.h"
#include "util/QESingleton.h"

#include <string>
#include <vector>
#include <set>
#include <string>

class QELogger;

class QELogManager : public QESingleton<QELogManager>
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

    QE_API void Load();
    QE_API void Unload();
    QE_API void Log(QE_IN const std::string& filePath,
                    QE_IN unsigned int lineNumber,
                    QE_IN const std::string& msg,
                    QE_IN unsigned int level);
    QE_API void SetFilter(QE_IN_OPT const std::vector<std::string> const* filterList);
    
private:

    std::string filterMessage_(const std::string& file,
                               unsigned int lineNumber,
                               const std::string& msg,
                               unsigned int level);
    bool isFileFiltered_(const std::string& file);

private:

    typedef std::string FileName;
    typedef std::string Extension;
    typedef std::set<FileName,Extension> FilterList;
    typedef std::vector<QELogger*> LoggerList;

private:

    FilterList filterList_;
    LoggerList loggerList_;
};

#endif