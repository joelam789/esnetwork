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

#ifndef _ESN_IOHANDLER_H_
#define _ESN_IOHANDLER_H_

#include "IoBuffer.h"
#include "Session.h"

namespace esnlib
{
/// IO Handler class (interface)
class IoHandler
{
private:

protected:

public:

    IoHandler();
    virtual ~IoHandler();

    /// The callback function for the Read event
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the useful data read
    /// @return The flags of the event processing: 1 = async, 2 = concurrent, 3 = both 1 and 2
    virtual int OnRead(SessionPtr session, IoBufferPtr data) = 0;

    /// The callback function for the Write event
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the data written
    /// @return The flags of the event processing: 1 = async, 2 = concurrent, 3 = both 1 and 2
    virtual int OnWrite(SessionPtr session, IoBufferPtr data) = 0;

    /// The callback function for the Connect event
    ///
    /// @param session Current session
    virtual void OnConnect(SessionPtr session) = 0;

    /// The callback function for the Disconnect event
    ///
    /// @param session Current session
    virtual void OnDisconnect(SessionPtr session) = 0;

    /// The callback function for the Idle event
    ///
    /// @param session Current session
    /// @param idleType The idle type (1 for read idle, 2 for write idle, 0 for both)
    virtual void OnIdle(SessionPtr session, int idleType) = 0;

    /// The callback function for the Error event
    ///
    /// @param session Current session
    /// @param errorType The error type (1 for read error, 2 for write error, 0 for connect error)
    /// @param errorCode The error code (if error type == 0 and error code == 0, that means connect timeout)
    /// @param errorMsg The error message
    virtual void OnError(SessionPtr session, int errorType, int errorCode, const std::string& errorMsg) = 0;

};

typedef boost::shared_ptr<IoHandler> IoHandlerPtr;

}

#endif // IOHANDLER_H

