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

#ifndef _ESN_STRINGHANDLER_H_
#define _ESN_STRINGHANDLER_H_

#include <vector>
#include <string>

#include "IoHandler.h"

namespace esnlib
{

/// String Handler class, an event handler who can handle network IO events for string message(ends with specific char)
class StringHandler : public IoHandler
{
public:
    StringHandler();
    virtual ~StringHandler();

    /// The callback function for the OnRead event
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the useful data read
    /// @return Whether need to take back the memory of the useful data
    virtual int OnRead(SessionPtr session, IoBufferPtr data);

    /// The callback function for the OnWrite event
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the data written
    /// @return Whether need to take back the memory of the data written
    virtual int OnWrite(SessionPtr session, IoBufferPtr data);

    /// The callback function for the OnConnect event
    ///
    /// @param session Current session
    virtual void OnConnect(SessionPtr session);

    /// The callback function for the OnDisconnect event
    ///
    /// @param session Current session
    virtual void OnDisconnect(SessionPtr session);

    /// The callback function for the OnIdle event
    ///
    /// @param session Current session
    /// @param idleType The idle type (1 for read idle, 2 for write idle, 0 for both)
    virtual void OnIdle(SessionPtr session, int idleType);

    /// The callback function for the OnError event
    ///
    /// @param session Current session
    /// @param errorType The error type (1 for read error, 2 for write error, 0 for connect error)
    /// @param errorCode The error code (if error type == 0 and error code == 0, that means connect timeout)
    /// @param errorMsg The error message
    virtual void OnError(SessionPtr session, int errorType, int errorCode, const std::string& errorMsg);

    /// Handle incoming string
    ///
    /// @param session Current session
    /// @param str The incoming string
    virtual void HandleString(SessionPtr session, const std::string& str);

    /// Split the string
    ///
    /// @param src The original string
    /// @param delims The delimiters
    /// @param strs The vector used to save the strings after splitting
    /// @return How many strings you will get after splitting
    static int SplitStr(const std::string& src, const std::string& delims, std::vector<std::string>& strs);

    /// Trim the string
    ///
    /// @param source The original string
    /// @param spacechars The space characters
    /// @return The string after trimming
    static std::string TrimStr(std::string const& source, char const* spacechars = " \t\r\n");

    /// Get current date and time in string format
    ///
    /// @return Current date and time in string format
    static std::string GetDateTimeStr();

    /// Get current time in string format
    ///
    /// @return Current time in string format
    static std::string GetTimeStr();

protected:
private:
};

typedef boost::shared_ptr<StringHandler> StringHandlerPtr;

}
#endif // _ESN_STRINGHANDLER_H_

