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

#ifndef _ESN_IOBUFFERMANAGER_H_
#define _ESN_IOBUFFERMANAGER_H_

#include "IoBuffer.h"
#include "BufferManager.h"

namespace esnlib
{
/// IO Buffer Manager, it will work like a memory pool
class IoBufferManager : public BufferManager
{
public:
    IoBufferManager();
    virtual ~IoBufferManager();

    /// Get a free buffer from the pool
    ///
    /// @return The pointer of the free buffer
    virtual BufferPtr GetFreeBuffer() = 0;

    /// Take the buffer back to the pool
    ///
    /// @param buf The buffer
    virtual void TakeBack(BufferPtr buf) = 0;

    /// Get a free IO buffer from the pool
    ///
    /// @param bufsize The size of the buffer
    /// @return The pointer of the free IO buffer
    virtual IoBufferPtr GetFreeBuffer(int bufsize) = 0;

    /// Take the IO buffer back to the pool
    ///
    /// @param buf The pointer of the IO buffer
    virtual void TakeBack(IoBufferPtr buf) = 0;

protected:
private:
};

typedef boost::shared_ptr<IoBufferManager> IoBufferManagerPtr;


/** \addtogroup MemoryPool
 *  @{
 */

/// Create a buffer pool with initial number of the buffers and the default size of every buffer
///
/// @param itemcount The initial number of the buffers
/// @param bufsize The default size of every buffer
/// @return The pointer of the buffer pool
IoBufferManagerPtr CreateIoBufferManager(int itemcount = 128, int bufsize = 8192);

/** @} */

}

#endif // _ESN_IOBUFFERMANAGER_H_

