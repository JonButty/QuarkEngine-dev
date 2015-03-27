#include "stdafx.h"
#include "QELogManager.h"
#include "QEConsoleLogger.h"

#include <sstream>

/*
QELogManager::QELogManager()
{
    loggerList_.push_back(new QEConsoleLogger());
}
*/
QELogManager::~QELogManager()
{
    LoggerList::iterator it = loggerList_.begin();

    while(it != loggerList_.end())
    {
        delete *it;
        ++it;
    }
    loggerList_.clear();
}

void QELogManager::Load()
{
    LoggerList::iterator it = loggerList_.begin();
    
    while(it != loggerList_.end())
    {
        (*it)->Load();
        ++it;
    }
}

void QELogManager::Unload()
{
    LoggerList::iterator it = loggerList_.begin();

    while(it != loggerList_.end())
    {
        (*it)->Unload();
        ++it;
    }
}

/*!*****************************************************************************

\name   Log
\param	const std::string & filePath
\param	unsigned int lineNumber
\param	const std::string & msg
\param	unsigned int level
\return void

\brief	Logs message if it passes through filter settings by passing it through
        all loggers registered.

*******************************************************************************/
void QELogManager::Log(const std::string& filePath,
                       unsigned int lineNumber,
                       const std::string& msg,
                       unsigned int level)
{
    std::string filteredMsg = filterMessage_(filePath,lineNumber,msg,level);

    if(filteredMsg.size() > 0)
    {
        LoggerList::iterator it = loggerList_.begin();

        while(it != loggerList_.end())
        {
            (*it)->Log(msg);
            ++it;
        }
    }
}

void QELogManager::SetFilter(const std::vector<std::string> const* filterList)
{

}

//------------------------------------------------------------------------------

std::string QELogManager::filterMessage_(const std::string& file,
                                         unsigned int lineNumber,
                                         const std::string& msg,
                                         unsigned int level)
{
    if(!isFileFiltered_(file))
        return std::string();

    std::stringstream ss;

    switch(level)
    {
    case L_VERBOSE:
        ss << "[QELog V] ";
        break;

    case L_WARNING:
        ss << "[QELog W] ";
        break;

    case L_ERROR:
        ss << "[QELog E] ";
        break;
    }

    ss << file << " (" << lineNumber << "): " << msg;
    return ss.str();
}

/*!*****************************************************************************

\todo Add functionality for wildcard character
\todo Move this into a file system as a static function

*******************************************************************************/
bool QELogManager::isFileFiltered_(const std::string& file)
{
    return filterList_.find(file) != filterList_.end();
}