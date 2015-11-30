#ifndef _ESN_LOGMANAGER_H_
#define _ESN_LOGMANAGER_H_

#include "Logger.h"

namespace esnlib
{

/// Global log manager

class LogManager
{
public:

    static const int LOG_LEVEL_TRACE   = 0;
	static const int LOG_LEVEL_DEBUG   = 1;
	static const int LOG_LEVEL_INFO    = 2;
	static const int LOG_LEVEL_WARNING = 3;
	static const int LOG_LEVEL_ERROR   = 4;
	static const int LOG_LEVEL_FATAL   = 5;

    LogManager();
    virtual ~LogManager();

    /// Init global log function
    ///
    /// @param defaultloggername The default logger's name
    /// @param defaultlogpath The default path where the log files will be stored
    static void Init(const std::string& defaultloggername = "", const std::string& defaultlogpath = "");

    static LoggerPtr AddLogger(const std::string& loggername, const std::string& logpath = "");
    static void RemoveLogger(const std::string& loggername);
    static LoggerPtr GetLogger(const std::string& loggername);
    static LoggerPtr GetDefaultLogger();
    static void Clear();

    /// Set minimum log level
    ///
    /// @param level The minimum level
    static void SetLogLevel(int level);

    /// Get minimum log level
    ///
    /// @return The minimum log level
    static int GetLogLevel();

    /// Log message for tracing
    ///
    /// @param msg The message
    static void Trace(const std::string& msg);
    static void Trace(const std::string& loggername, const std::string& msg);

    /// Log message for debugging
    ///
    /// @param msg The message
    static void Debug(const std::string& msg);
    static void Debug(const std::string& loggername, const std::string& msg);

    /// Log message for informing
    ///
    /// @param msg The message
    static void Info(const std::string& msg);
    static void Info(const std::string& loggername, const std::string& msg);

    /// Log message for warning
    ///
    /// @param msg The message
    static void Warning(const std::string& msg);
    static void Warning(const std::string& loggername, const std::string& msg);

    /// Log message of error
    ///
    /// @param msg The message
    static void Error(const std::string& msg);
    static void Error(const std::string& loggername, const std::string& msg);

    /// Log message of fatal error
    ///
    /// @param msg The message
    static void Fatal(const std::string& msg);
    static void Fatal(const std::string& loggername, const std::string& msg);

protected:
private:



};

}

#endif // _ESN_LOGMANAGER_H_
