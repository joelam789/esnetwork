#include <map>
#include <string>

#ifndef ESN_WITHOUT_BOOST_LOG

#include <fstream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#else

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>

#endif // ESN_WITHOUT_BOOST_LOG


#include "LogManager.h"

using namespace esnlib;

static bool g_initlog = false;
static LoggerPtr g_defaultlog;
static std::string g_defaultlogname = "esn";
static std::string g_defaultlogpath = ".";
static int g_loglevel = 0;
static boost::mutex g_logmapmutex;
static std::map<std::string, LoggerPtr> g_logmap;

LoggerPtr AddLoggerWithName(const std::string& loggername, const std::string& logpath)
{
    LoggerPtr logger;
    if(g_initlog)
    {
        std::map<std::string, LoggerPtr>::iterator logitem;
        boost::unique_lock<boost::mutex> lock(g_logmapmutex);
        logitem = g_logmap.find(loggername);
        if(logitem != g_logmap.end()) logger = logitem->second;
        else
        {
            logger = CreateLogger(loggername, logpath);
            g_logmap.insert(std::map<std::string, LoggerPtr>::value_type(loggername, logger));
        }
    }
    return logger;
}

LoggerPtr GetLoggerByName(const std::string& loggername)
{
    LoggerPtr logger;
    if(g_initlog)
    {
        std::map<std::string, LoggerPtr>::iterator logitem;
        boost::unique_lock<boost::mutex> lock(g_logmapmutex);
        logitem = g_logmap.find(loggername);
        if(logitem != g_logmap.end()) logger = logitem->second;
    }
    return logger;
}

bool RemoveLoggerByName(const std::string& loggername)
{
    bool removed = false;
    if(g_initlog)
    {
        boost::unique_lock<boost::mutex> lock(g_logmapmutex);
        g_logmap.erase(loggername);
        removed = true;
    }
    return removed;
}

void RemoveAllLoggers()
{
    boost::unique_lock<boost::mutex> lock(g_logmapmutex);
    g_logmap.clear();
}

LogManager::LogManager()
{
    //ctor
}

LogManager::~LogManager()
{
    //dtor
}

#ifndef ESN_WITHOUT_BOOST_LOG

void LogManager::Init(const std::string& defaultloggername, const std::string& defaultlogpath)
{
    if(g_initlog || g_defaultlog) return;

    g_defaultlog = CreateLogger(defaultloggername, defaultlogpath);

    if(g_defaultlog)
    {
        g_initlog = true;
        g_defaultlogname = defaultloggername;
        g_defaultlogpath = defaultlogpath;
    }

    if(g_initlog)
    {
        g_loglevel = 0;
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();
        if(core) core->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
    }
}

void LogManager::SetLogLevel(int level)
{
    boost::shared_ptr<boost::log::core> core = boost::log::core::get();

    if(core)
    {
        switch(level)
        {
            case LOG_LEVEL_TRACE:
                g_loglevel = LOG_LEVEL_TRACE;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
                break;
            case LOG_LEVEL_DEBUG:
                g_loglevel = LOG_LEVEL_DEBUG;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
                break;
            case LOG_LEVEL_INFO:
                g_loglevel = LOG_LEVEL_INFO;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
                break;
            case LOG_LEVEL_WARNING:
                g_loglevel = LOG_LEVEL_WARNING;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
                break;
            case LOG_LEVEL_ERROR:
                g_loglevel = LOG_LEVEL_ERROR;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
                break;
            case LOG_LEVEL_FATAL:
                g_loglevel = LOG_LEVEL_FATAL;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::fatal);
                break;
            default:
                g_loglevel = LOG_LEVEL_INFO;
                core->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
        }
    }
}

#else

void LogManager::Init(const std::string& defaultloggername, const std::string& defaultlogpath)
{
    if(g_initlog || g_defaultlog) return;

    g_defaultlog = CreateLogger(defaultloggername, defaultlogpath);

    if(g_defaultlog)
    {
        g_initlog = true;
        g_defaultlogname = defaultloggername;
        g_defaultlogpath = defaultlogpath;
    }

    if(g_initlog)
    {
        g_loglevel = 0;
    }
}

void LogManager::SetLogLevel(int level)
{
    switch(level)
    {
        case LOG_LEVEL_TRACE:
            g_loglevel = LOG_LEVEL_TRACE;
            break;
        case LOG_LEVEL_DEBUG:
            g_loglevel = LOG_LEVEL_DEBUG;
            break;
        case LOG_LEVEL_INFO:
            g_loglevel = LOG_LEVEL_INFO;
            break;
        case LOG_LEVEL_WARNING:
            g_loglevel = LOG_LEVEL_WARNING;
            break;
        case LOG_LEVEL_ERROR:
            g_loglevel = LOG_LEVEL_ERROR;
            break;
        case LOG_LEVEL_FATAL:
            g_loglevel = LOG_LEVEL_FATAL;
            break;
        default:
            g_loglevel = LOG_LEVEL_INFO;
    }
}

#endif // ESN_WITHOUT_BOOST_LOG

LoggerPtr LogManager::AddLogger(const std::string& loggername, const std::string& logpath)
{
    return AddLoggerWithName(loggername, logpath);
}

void LogManager::RemoveLogger(const std::string& loggername)
{
    RemoveLoggerByName(loggername);
}

LoggerPtr LogManager::GetLogger(const std::string& loggername)
{
    return GetLoggerByName(loggername);
}

LoggerPtr LogManager::GetDefaultLogger()
{
    return g_defaultlog;
}

void LogManager::Clear()
{
    RemoveAllLoggers();
}

int LogManager::GetLogLevel()
{
    return g_loglevel;
}

void LogManager::Trace(const std::string& msg)
{
    if(g_initlog && g_defaultlog)
    {
        g_defaultlog->Trace(msg);
    }
    else
    {
        if (LOG_LEVEL_TRACE >= g_loglevel) std::cout << msg << std::endl;
    }
}
void LogManager::Trace(const std::string& loggername, const std::string& msg)
{
    LoggerPtr logger = GetLoggerByName(loggername);
    if(logger) logger->Trace(msg);
}

void LogManager::Debug(const std::string& msg)
{
    if(g_initlog && g_defaultlog)
    {
        g_defaultlog->Debug(msg);
    }
    else
    {
        if (LOG_LEVEL_DEBUG >= g_loglevel) std::cout << msg << std::endl;
    }
}
void LogManager::Debug(const std::string& loggername, const std::string& msg)
{
    LoggerPtr logger = GetLoggerByName(loggername);
    if(logger) logger->Debug(msg);
}

void LogManager::Info(const std::string& msg)
{
    if(g_initlog && g_defaultlog)
    {
        g_defaultlog->Info(msg);
    }
    else
    {
        if (LOG_LEVEL_INFO >= g_loglevel) std::cout << msg << std::endl;
    }
}
void LogManager::Info(const std::string& loggername, const std::string& msg)
{
    LoggerPtr logger = GetLoggerByName(loggername);
    if(logger) logger->Info(msg);
}

void LogManager::Warning(const std::string& msg)
{
    if(g_initlog && g_defaultlog)
    {
        g_defaultlog->Warning(msg);
    }
    else
    {
        if (LOG_LEVEL_WARNING >= g_loglevel) std::cout << msg << std::endl;
    }
}
void LogManager::Warning(const std::string& loggername, const std::string& msg)
{
    LoggerPtr logger = GetLoggerByName(loggername);
    if(logger) logger->Warning(msg);
}

void LogManager::Error(const std::string& msg)
{
    if(g_initlog && g_defaultlog)
    {
        g_defaultlog->Error(msg);
    }
    else
    {
        if (LOG_LEVEL_ERROR >= g_loglevel) std::cout << msg << std::endl;
    }
}
void LogManager::Error(const std::string& loggername, const std::string& msg)
{
    LoggerPtr logger = GetLoggerByName(loggername);
    if(logger) logger->Error(msg);
}

void LogManager::Fatal(const std::string& msg)
{
    if(g_initlog && g_defaultlog)
    {
        g_defaultlog->Fatal(msg);
    }
    else
    {
        if (LOG_LEVEL_FATAL >= g_loglevel) std::cout << msg << std::endl;
    }
}
void LogManager::Fatal(const std::string& loggername, const std::string& msg)
{
    LoggerPtr logger = GetLoggerByName(loggername);
    if(logger) logger->Fatal(msg);
}
