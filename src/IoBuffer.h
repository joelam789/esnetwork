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

#ifndef _ESN_IOBUFFER_H_
#define _ESN_IOBUFFER_H_

#include <deque>

#include "Buffer.h"
#include "Session.h"

#ifndef ESN_MAX_IOBUF_SIZE
#define ESN_MAX_IOBUF_SIZE     1024 * 1024 * 256
#endif

#ifndef ESN_DEFAULT_IOBUF_SIZE
#define ESN_DEFAULT_IOBUF_SIZE 2048
#endif

namespace esnlib
{

/// IO Buffer class
class IoBuffer : public Buffer
{
public:

    /// Constructor function
    IoBuffer();

    /// Constructor function
    ///
    /// @param buf The raw buffer pointer
    /// @param bufsize The raw buffer size
    IoBuffer(const char* buf, int bufsize);

    /// Constructor function
    ///
    /// @param size The buffer size
    explicit IoBuffer(int size);

    /// Constructor function
    ///
    /// @param src The other buffer whose content will be copied
    explicit IoBuffer(const IoBuffer& src);

    /// Destructor function
    virtual ~IoBuffer();

    /// Get the raw buffer
    ///
    /// @return The pointer of the raw buffer
    const char* data() const;

    /// Get the raw buffer
    ///
    /// @return The pointer of the raw buffer
    char* data();

    /// Get the buffer size
    ///
    /// @return The buffer size
    int size() const;

    /// Set the buffer size
    ///
    /// @param value The new buffer size we ask for
    /// @return The actual buffer size got finally
    int size(int value);

    /// Get the buffer flag
    ///
    /// @return The buffer flag
    int flag() const;

    /// Set the buffer flag
    ///
    /// @param value The buffer flag
    void flag(int value);

    /// Get the buffer code
    ///
    /// @return The buffer code
    int code() const;

    /// Set the buffer code
    ///
    /// @param value The buffer code
    void code(int value);

    /// Get the buffer type
    ///
    /// @return The buffer type
    int type() const;

    /// Set the buffer type
    ///
    /// @param value The buffer type
    void type(int value);

    /// Get the buffer state
    ///
    /// @return The buffer state
    int state() const;

    /// Set the buffer state
    ///
    /// @param value The buffer state
    void state(int value);

    /// Check whether the buffer size is specific
	///
	/// @return Return false if the buffer size is specific
    bool anysize() const;

    /// Get the session
    ///
    /// @return The pointer of the session
    SessionPtr session();

    /// Set the session
    ///
    /// @param value The pointer of the session
    void session(SessionPtr value);

    /// Get the read position
    ///
    /// @return The read position
    int GetReadPos();

    /// Set the read position
    ///
    /// @param pos The read position
    void SetReadPos(int pos);

    /// Get the write position
    ///
    /// @return The write position
    int GetWritePos();

    /// Set the write position
    ///
    /// @param pos The write position
    void SetWritePos(int pos);

    /// Read a byte value at current read position from the raw buffer
    ///
    /// @return The byte value
    char GetByte();

    /// Read a short value at current read position from the raw buffer
    ///
    /// @return The short value
    short GetShort();

    /// Read an integer value at current read position from the raw buffer
    ///
    /// @return The integer value
    int GetInt();

    /// Read a double value at current read position from the raw buffer
    ///
    /// @return The double value
    double GetDouble();

    /// Read bytes at current read position from the raw buffer
    ///
    /// @param buf The other buffer used to save the bytes
    /// @param len How many bytes you want to read
    /// @return The total number of the bytes read
    int GetBuf(char* buf, int len);

    /// Read a string value at current read position from the raw buffer
    ///
    /// @param len The length of the string
    /// @return The string value
    std::string GetStr(int len);

    /// Write a byte value into the raw buffer (at current write position)
    ///
    /// @param value The byte value
    void PutByte(char value);

    /// Write a short value into the raw buffer (at current write position)
    ///
    /// @param value The short value
    void PutShort(int value);

    /// Write an integer value into the raw buffer (at current write position)
    ///
    /// @param value The integer value
    void PutInt(int value);

    /// Write a double value into the raw buffer (at current write position)
    ///
    /// @param value The double value
    void PutDouble(double value);

    /// Write bytes into the raw buffer (at current write position)
    ///
    /// @param buf The pointer of the bytes
    /// @param len How many bytes
    void PutBuf(char* buf, int len);

    /// Write a string value into the raw buffer (at current write position)
    ///
    /// @param value The string value
    void PutStr(const std::string& value);

protected:

private:
    char* m_data;

    int m_size;
    int m_max;

    int m_code;
    int m_flag;
    int m_type;
    int m_state;

    int m_default;

    int m_readpos;
    int m_writepos;

    SessionRef m_session;

};

typedef boost::shared_ptr<IoBuffer> IoBufferPtr;

typedef std::deque<IoBufferPtr> IoBufferQueue;

}

#endif // _ESN_IOBUFFER_H_

