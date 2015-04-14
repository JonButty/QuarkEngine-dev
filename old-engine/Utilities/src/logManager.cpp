#include "logManager.h"
#include "logger.h"

namespace Util
{
    LogManager::LogManager()
    {
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
            loggers_[i] = 0;
    }

    LogManager::~LogManager()
    {
    }

    void LogManager::Load()
    {
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
        {
            if(loggers_[i])
            {
                LoggerList::iterator it = loggers_[i]->begin();
                LoggerList::iterator end = loggers_[i]->end();

                for(; it != end; ++it)
                    (*it)->Load();
            }
        }
    }

    void LogManager::RegisterLogger( Logger* logger )
    {
        if(logger)
        {
            if(!loggers_[logger->Type()])
                loggers_[logger->Type()] = new LoggerList();
            loggers_[logger->Type()]->push_back(logger);
        }
    }

    void LogManager::EnableLogger( unsigned int type,
                                   bool enable )
    {
        LoggerList::iterator it = loggers_[type]->begin();
        LoggerList::iterator end = loggers_[type]->end();

        for(; it != end; ++it)
            (*it)->Enable(enable);
    }

    void LogManager::Log( const std::string& message )
    {
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
            Log(i,message);
    }

    void LogManager::Log( const std::wstring& message )
    {
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
            Log(i,message);
    }

    void LogManager::Log( unsigned int type,
                          const std::string& message )
    {
        LoggerList::iterator it = loggers_[type]->begin();
        LoggerList::iterator end = loggers_[type]->end();
        
        for(; it != end; ++it)
            (*it)->Log(message);
    }

    void LogManager::Log( unsigned int type,
                          const std::wstring& message )
    {
        LoggerList::iterator it = loggers_[type]->begin();
        LoggerList::iterator end = loggers_[type]->end();

        for(; it != end; ++it)
            (*it)->Log(message);
    }

    void LogManager::Unload()
    {
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
        {
            if(loggers_[i])
            {
                LoggerList::iterator it = loggers_[i]->begin();
                LoggerList::iterator end = loggers_[i]->end();

                for(; it != end; ++it)
                    (*it)->Unload();
                delete loggers_[i];
                loggers_[i] = 0;
            }
        }
    }
}