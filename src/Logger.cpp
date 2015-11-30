/*
-----------------------------------------------------------------------------
This source file is part of "esnetwork" library.
It is licensed under the terms of the BSD license.
For the latest info, see http://esnetwork.sourceforge.net

Copyright (c) 2012-2013 Lin Jia Jun (Joe Lam)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#ifndef ESN_WITHOUT_BOOST_LOG

#include <fstream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#else

#include <iostream>

#endif // ESN_WITHOUT_BOOST_LOG


#include "Logger.h"

#ifndef ESN_MAX_LOG_FILE_SIZE
#define ESN_MAX_LOG_FILE_SIZE   1024 * 1024 * 10
#endif


#ifndef ESN_WITHOUT_BOOST_LOG
namespace boost_log = boost::log;
namespace boost_log_src = boost::log::sources;
namespace boost_log_expr = boost::log::expressions;
namespace boost_log_sinks = boost::log::sinks;
namespace boost_log_keywords = boost::log::keywords;
namespace boost_file = boost::filesystem;
#endif // ESN_WITHOUT_BOOST_LOG

using namespace esnlib;

static bool g_firstlogger = true;

Logger::Logger()
{
    //ctor
}

Logger::~Logger()
{
    //dtor
}

#ifndef ESN_WITHOUT_BOOST_LOG

class LoggerImpl : public Logger
{
public:
    LoggerImpl();
    LoggerImpl(const std::string& name, const std::string& path);
    virtual ~LoggerImpl();

    /// Log message for tracing
    ///
    /// @param msg The message
    virtual void Trace(const std::string& msg);

    /// Log message for debugging
    ///
    /// @param msg The message
    virtual void Debug(const std::string& msg);

    /// Log message for informing
    ///
    /// @param msg The message
    virtual void Info(const std::string& msg);

    /// Log message for warning
    ///
    /// @param msg The message
    virtual void Warning(const std::string& msg);

    /// Log message of error
    ///
    /// @param msg The message
    virtual void Error(const std::string& msg);

    /// Log message of fatal error
    ///
    /// @param msg The message
    virtual void Fatal(const std::string& msg);

    /// Set minimum log level
    ///
    /// @param level The minimum level
    //virtual void SetLogLevel(int level);

protected:

    typedef boost_log_sinks::synchronous_sink< boost_log_sinks::text_ostream_backend > text_sink;
    typedef boost_log_sinks::synchronous_sink< boost_log_sinks::text_file_backend > file_sink;

    void Init(const std::string& name, const std::string& path);

private:

    std::string m_name;
    std::string m_path;

    boost::shared_ptr< text_sink > m_strsink;
    boost::shared_ptr< file_sink > m_filesink;
    boost_log_src::severity_channel_logger_mt<boost_log::trivial::severity_level, std::string> m_log;

};

void LoggerImpl::Init(const std::string& name, const std::string& path)
{
    if(m_strsink || m_filesink) return;

    m_name = name;
    m_path = path;

    if(m_name.length() <= 0) m_name = "esn";
    //if(m_path.length() <= 0) m_path = ".";

    if(m_path.length() > 0)
    {
        boost_file::path logpath(m_path);
        if(!boost_file::exists(logpath) || !boost_file::is_directory(logpath))
        {
            std::cout << "Invalid log path: " << m_path << std::endl;
            //m_path = ".";
            m_path = "";
        }
    }

    m_log.channel(m_name);

    boost::shared_ptr<boost_log::core> core = boost_log::core::get();

    // text log
    boost::shared_ptr< boost_log_sinks::text_ostream_backend > strbackend =
        boost::make_shared< boost_log_sinks::text_ostream_backend >();

    strbackend->add_stream(boost::shared_ptr< std::ostream >(&std::clog));
    strbackend->auto_flush(true);

    m_strsink = boost::make_shared< text_sink >(strbackend);

    //m_strsink->set_filter(logging::trivial::severity >= logging::trivial::info);
    m_strsink->set_filter(boost_log_expr::attr<std::string>("Channel") == m_name);
    m_strsink->set_formatter (
         boost_log_expr::format("[%1%](%2%.%3%): %4%")
         % boost_log_expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
         % boost_log_expr::attr<std::string>("Channel")
         % boost_log::trivial::severity
         % boost_log_expr::smessage
    );

    core->add_sink(m_strsink);

    if(m_path.length() > 0)
    {
        // file log
        boost::shared_ptr< boost_log_sinks::text_file_backend > filebackend =
            boost::make_shared< boost_log_sinks::text_file_backend > (
                boost_log_keywords::file_name = m_path + "/" + m_name + "_%Y%m%d_%3N.log",
                boost_log_keywords::rotation_size = ESN_MAX_LOG_FILE_SIZE,
                boost_log_keywords::time_based_rotation = boost_log_sinks::file::rotation_at_time_point(0, 0, 0)
        );

        m_filesink = boost::make_shared< file_sink >(filebackend);

        //m_filesink->set_filter(boost_log::trivial::severity >= boost_log::trivial::info);
        m_filesink->set_filter(boost_log_expr::attr<std::string>("Channel") == m_name);
        m_filesink->set_formatter (
             boost_log_expr::format("[%1%](%2%.%3%): %4%")
             % boost_log_expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
             % boost_log_expr::attr<std::string>("Channel")
             % boost_log::trivial::severity
             % boost_log_expr::smessage
        );

        core->add_sink(m_filesink);
    }

    if(g_firstlogger)
    {
        boost_log::add_common_attributes();
        g_firstlogger = false;
    }
}

LoggerImpl::LoggerImpl()
{
    Init("", "");
}
LoggerImpl::LoggerImpl(const std::string& name, const std::string& path)
{
    Init(name, path);
}
LoggerImpl::~LoggerImpl()
{
    boost::shared_ptr<boost_log::core> core = boost_log::core::get();
    if(core)
    {
        if(m_strsink) core->remove_sink(m_strsink);
        if(m_filesink) core->remove_sink(m_filesink);
    }
}

void LoggerImpl::Trace(const std::string& msg)
{
    try { BOOST_LOG_SEV(m_log, boost_log::trivial::trace) << msg; }
    catch(...) { }
}

void LoggerImpl::Debug(const std::string& msg)
{
    try { BOOST_LOG_SEV(m_log, boost_log::trivial::debug) << msg; }
    catch(...) { }
}

void LoggerImpl::Info(const std::string& msg)
{
    try { BOOST_LOG_SEV(m_log, boost_log::trivial::info) << msg; }
    catch(...) { }
}

void LoggerImpl::Warning(const std::string& msg)
{
    try { BOOST_LOG_SEV(m_log, boost_log::trivial::warning) << msg; }
    catch(...) { }
}

void LoggerImpl::Error(const std::string& msg)
{
    try { BOOST_LOG_SEV(m_log, boost_log::trivial::error) << msg; }
    catch(...) { }
}

void LoggerImpl::Fatal(const std::string& msg)
{
    try { BOOST_LOG_SEV(m_log, boost_log::trivial::fatal) << msg; }
    catch(...) { }
}

#else

class LoggerImpl : public Logger
{
public:
    LoggerImpl();
    LoggerImpl(const std::string& name, const std::string& path);
    virtual ~LoggerImpl();

    /// Log message for tracing
    ///
    /// @param msg The message
    virtual void Trace(const std::string& msg);

    /// Log message for debugging
    ///
    /// @param msg The message
    virtual void Debug(const std::string& msg);

    /// Log message for informing
    ///
    /// @param msg The message
    virtual void Info(const std::string& msg);

    /// Log message for warning
    ///
    /// @param msg The message
    virtual void Warning(const std::string& msg);

    /// Log message of error
    ///
    /// @param msg The message
    virtual void Error(const std::string& msg);

    /// Log message of fatal error
    ///
    /// @param msg The message
    virtual void Fatal(const std::string& msg);

    /// Set minimum log level
    ///
    /// @param level The minimum level
    //virtual void SetLogLevel(int level);

protected:

    void Init(const std::string& name, const std::string& path);

private:

    std::string m_name;
    std::string m_path;

};

void LoggerImpl::Init(const std::string& name, const std::string& path)
{
    m_name = name;
    m_path = path;
}

LoggerImpl::LoggerImpl()
{
    Init("", "");
}
LoggerImpl::LoggerImpl(const std::string& name, const std::string& path)
{
    Init(name, path);
}
LoggerImpl::~LoggerImpl()
{
    m_name = "";
    m_path = "";
}

void LoggerImpl::Trace(const std::string& msg)
{
    try { std::cout << "[TRACE] " << msg << std::endl; }
    catch(...) { }
}

void LoggerImpl::Debug(const std::string& msg)
{
    try { std::cout << "[DEBUG] " << msg << std::endl; }
    catch(...) { }
}

void LoggerImpl::Info(const std::string& msg)
{
    try { std::cout << "[INFO] " << msg << std::endl; }
    catch(...) { }
}

void LoggerImpl::Warning(const std::string& msg)
{
    try { std::cout << "[WARNING] " << msg << std::endl; }
    catch(...) { }
}

void LoggerImpl::Error(const std::string& msg)
{
    try { std::cout << "[ERROR] " << msg << std::endl; }
    catch(...) { }
}

void LoggerImpl::Fatal(const std::string& msg)
{
    try { std::cout << "[FATAL] " << msg << std::endl; }
    catch(...) { }
}

#endif // ESN_WITHOUT_BOOST_LOG


esnlib::LoggerPtr esnlib::CreateLogger(const std::string& loggername, const std::string& logpath)
{
    esnlib::LoggerPtr logger(new LoggerImpl(loggername, logpath));
    return logger;
}
