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

#ifndef _ESN_MESSAGEHANDLER_H_
#define _ESN_MESSAGEHANDLER_H_

#include "IoHandler.h"
#include "WorkManager.h"

namespace esnlib
{

/// Common Message Handler class, an event handler who can handle network IO events for common message(format: header + body)
class MessageHandler : public IoHandler
{
public:

    /// Constructor function
    MessageHandler();

    /// Constructor function
    ///
    /// @param manager The pointer of the work manager (a thread pool manager)
    explicit MessageHandler(WorkManagerPtr manager);

    /// Constructor function
    ///
    /// @param manager The pointer of the work manager (a thread pool manager)
    /// @param work The pointer of the work (which provides the callback function for the threads)
    MessageHandler(WorkManagerPtr manager, WorkPtr work);

    /// Destructor function
    virtual ~MessageHandler();

    /// Get WorkManager (the thread pool)
    ///
    /// @return The WorkManager (pointer)
    WorkManagerPtr GetWorkManager();

    /// Get default work (the action that will be executed in threads)
    ///
    /// @return The DefaultWork (pointer)
    WorkPtr GetDefaultWork();

    /// Get the code from the buffer data
    ///
    /// @param data The buffer data
    /// @return The code
    virtual int GetCode(IoBuffer* data);

    /// Get the flag from the buffer data
    ///
    /// @param data The buffer data
    /// @return The flag
    virtual int GetFlag(IoBuffer* data);

    /// Get the integer key from the buffer data
    ///
    /// @param data The buffer data
    /// @return The integer key
    virtual int GetIntKey(IoBuffer* data);

    /// Get the string key from the buffer data
    ///
    /// @param data The buffer data
    /// @return The string key
    virtual std::string GetStrKey(IoBuffer* data);

    /// Check whether the message is an orderly message
    ///
    /// @param data The message data
    /// @return Return true if the message is orderly
    virtual bool IsOrderlyMessage(IoBuffer* data);

    /// Handle incoming data
    ///
    /// @param session Current session
    /// @param data The incoming data
    virtual void HandleMessage(SessionPtr session, IoBufferPtr data);

    /// The callback function for the Read event
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the useful data read
    /// @return The flags of the event processing: 1 = async, 2 = concurrent, 3 = both 1 and 2
    virtual int OnRead(SessionPtr session, IoBufferPtr data);

    /// The callback function for the Write event
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the data written
    /// @return The flags of the event processing: 1 = async, 2 = concurrent, 3 = both 1 and 2
    virtual int OnWrite(SessionPtr session, IoBufferPtr data);

    /// The callback function for the Connect event
    ///
    /// @param session Current session
    virtual void OnConnect(SessionPtr session);

    /// The callback function for the Disconnect event
    ///
    /// @param session Current session
    virtual void OnDisconnect(SessionPtr session);

    /// The callback function for the Idle event
    ///
    /// @param session Current session
    /// @param idleType The idle type (1 for read idle, 2 for write idle, 0 for both)
    virtual void OnIdle(SessionPtr session, int idleType);

    /// The callback function for the Error event
    ///
    /// @param session Current session
    /// @param errorType The error type (1 for read error, 2 for write error, 0 for connect error)
    /// @param errorCode The error code (if error type == 0 and error code == 0, that means connect timeout)
    /// @param errorMsg The error message
    virtual void OnError(SessionPtr session, int errorType, int errorCode, const std::string& errorMsg);

protected:

    WorkManagerPtr m_manager;
    WorkPtr m_work;

    /// The default callback function for the OnWrite event to process outgoing data asynchronously
    ///
    /// @param session Current session
    /// @param data The buffer pointer of the data written
    /// @return The flags of the event processing: 1 = async, 2 = concurrent, 3 = both 1 and 2
    int DefaultAsyncProcessOnWrite(SessionPtr session, IoBufferPtr data);

private:



};

}

#endif // MESSAGEHANDLER_H

