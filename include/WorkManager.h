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

#ifndef _ESN_WORKMANAGER_H_
#define _ESN_WORKMANAGER_H_

#include "Work.h"
#include "Buffer.h"
#include "BufferManager.h"

namespace esnlib
{

/// Work Manager class (interface), it will work like a thread pool
class WorkManager
{

public:

    WorkManager(){}
    virtual ~WorkManager(){}

    /// Stop running gently
    ///
    /// @return Current state of the work manager
    virtual int Stop() = 0;

    /// Force to stop anything
    ///
    /// @return Current state of the work manager
    virtual int Abort() = 0;

    /// Get current state of the work manager
    ///
    /// @return Current state of the work manager
    virtual int GetState() = 0;

    /// Get total number of the workers(threads)
    ///
    /// @return The total number of the workers(threads)
    virtual int GetWorkerCount() = 0;

    /// Get total number of the tasks
    ///
    /// @return The total number of the tasks
    virtual int GetTaskCount() = 0;

    /// Add a new work task
    ///
    /// @param work The work object, which will provide the callback function for the threads
    /// @param workdata The working data for the task
    /// @return The total number of the tasks
    virtual int AddWorkTask(WorkPtr work, BufferPtr workdata) = 0;

    /// Set Buffer Manager (memory pool)
    ///
    /// @param manager The pointer of the buffer manager
    virtual void SetBufferManager(BufferManagerPtr manager) = 0;

    /// Add a work object with an integer key
    ///
    /// @param key The integer key
    /// @param work The work object, which will provide the callback function for the threads
    /// @return The total number of the works mapped by integer keys
    virtual int PutWork(int key, WorkPtr work) = 0;

    /// Get a work object by an integer key
    ///
    /// @param key The integer key
    /// @return The pointer of the work object
    virtual WorkPtr GetWork(int key) = 0;

    /// Add a work object with a string key
    ///
    /// @param key The string key
    /// @param work The work object, which will provide the callback function for the threads
    /// @return The total number of the works mapped by string keys
    virtual int PutWork(const std::string& key, WorkPtr work) = 0;

    /// Get a work object by a string key
    ///
    /// @param key The string key
    /// @return The pointer of the work object
    virtual WorkPtr GetWork(const std::string& key) = 0;

};

typedef boost::shared_ptr<WorkManager> WorkManagerPtr;

/** \addtogroup ThreadPool
 *  @{
 */

/// Create a thread pool
///
/// @param minworkercount The minimum number of the threads ("0" means default)
/// @param maxworkercount The maximum number of the threads ("0" means default)
/// @param workstack Every thread's working stack size (in byte, "0" means default)
/// @return The pointer of the thread pool
WorkManagerPtr CreateWorkManager(int minworkercount = 0, int maxworkercount = 0, int workstack = 0);

/** @} */

}

#endif // WORKMANAGER_H

