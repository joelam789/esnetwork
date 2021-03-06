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

#ifndef _ESN_IOSERVICE_H_
#define _ESN_IOSERVICE_H_

#include <boost/shared_ptr.hpp>

namespace esnlib
{

/// IoService class, it will provide a thread running on IO port to process its IO events
class IoService
{
public:

    /// Get the pointer of the runner (a struct including a thread)
    ///
    /// @return The pointer of the runner
    virtual void * GetRunner() = 0;

    /// Get the pointer of the order sustainer (for orderly execution)
    ///
    /// @return The pointer of the order sustainer
    virtual void * GetOrderSustainer() = 0;

	/// Start the service (run a thread to process its IO events)
    ///
	virtual void Start() = 0;

	/// Stop the service
    ///
	virtual void Stop() = 0;

protected:

private:

};

typedef boost::shared_ptr<IoService> IoServicePtr;

/// Create IO Service
///
/// @param threadStackSize The thread's working stack size (in byte, "0" means default)
/// @return The pointer of the IO service
IoServicePtr CreateIoService(int threadStackSize = 0);

}

#endif // _ESN_IOSERVICE_H_

