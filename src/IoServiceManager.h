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

#ifndef _ESN_IOSERVICE_MANAGER_H_
#define _ESN_IOSERVICE_MANAGER_H_

#include "IoService.h"

namespace esnlib
{

/// a pool of IoService (IO thread pool)
class IoServiceManager
{
public:

    /// Get IoService 
    ///
    /// @return The pointer of the IoService
    virtual IoServicePtr GetService() = 0;

	/// Get pool size
    ///
	virtual int GetServiceCount() = 0;

protected:

private:

};

typedef boost::shared_ptr<IoServiceManager> IoServiceManagerPtr;

/** \addtogroup IoThreadPool
 *  @{
 */

/// Create IO thread pool
///
/// @param poolSize The pool size ("0" means default)
/// @param svcStackSize The stack size of every IO thread ("0" means default)
/// @return The pointer of the IO thread pool
IoServiceManagerPtr CreateIoServiceManager(int poolSize = 0, int svcStackSize = 0);

/** @} */

}

#endif // _ESN_IOSERVICE_MANAGER_H_

