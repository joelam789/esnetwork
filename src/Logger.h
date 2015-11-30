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

#ifndef _ESN_LOGGER_H_
#define _ESN_LOGGER_H_

#include <boost/shared_ptr.hpp>

namespace esnlib
{

/// Logger class (interface)

class Logger
{

public:
    Logger();
    virtual ~Logger();

    /// Log message for tracing
    ///
    /// @param msg The message
    virtual void Trace(const std::string& msg) = 0;

    /// Log message for debugging
    ///
    /// @param msg The message
    virtual void Debug(const std::string& msg) = 0;

    /// Log message for informing
    ///
    /// @param msg The message
    virtual void Info(const std::string& msg) = 0;

    /// Log message for warning
    ///
    /// @param msg The message
    virtual void Warning(const std::string& msg) = 0;

    /// Log message of error
    ///
    /// @param msg The message
    virtual void Error(const std::string& msg) = 0;

    /// Log message of fatal error
    ///
    /// @param msg The message
    virtual void Fatal(const std::string& msg) = 0;


protected:

private:

};

typedef boost::shared_ptr<Logger> LoggerPtr;

LoggerPtr CreateLogger(const std::string& loggername = "", const std::string& logpath = "");

}

#endif // _ESN_LOGGER_H_
