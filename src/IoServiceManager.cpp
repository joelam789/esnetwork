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

#include <vector>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "IoServiceManager.h"

using namespace esnlib;

class IoServiceManagerImpl : public IoServiceManager
{
public:

	explicit IoServiceManagerImpl(int poolsize = 0, int stacksize = 0);
    virtual ~IoServiceManagerImpl();

    virtual IoServicePtr GetService();

	virtual int GetServiceCount();

protected:

private:

	std::vector<IoServicePtr> m_svclist;

	boost::mutex m_mutex;

	int m_poolsize;
	int m_stacksize;

	int m_index;
};


IoServiceManagerImpl::IoServiceManagerImpl(int poolsize, int stacksize)
{
	m_index = 0;

	m_stacksize = stacksize;
    if (m_stacksize < 0) m_stacksize = 0;

	m_poolsize = poolsize;

	if (m_poolsize <= 0) m_poolsize = boost::thread::hardware_concurrency();
    if (m_poolsize <= 0) m_poolsize = 1;

	if (m_poolsize > 0)
	{
		for(int i=0; i<m_poolsize; i++)
		{
			IoServicePtr svc = CreateIoService(m_stacksize);
			m_svclist.push_back(svc);
		}
		for(int i=0; i<m_poolsize; i++)
        {
            m_svclist[i]->Start();
        }
	}
}

IoServiceManagerImpl::~IoServiceManagerImpl()
{
	boost::unique_lock<boost::mutex> lock(m_mutex);
	for(int i=0; i<m_poolsize; i++)
	{
		m_svclist[i]->Stop();
	}
	m_svclist.clear();
	m_poolsize = 0;
	m_index = 0;
}

IoServicePtr IoServiceManagerImpl::GetService()
{
	IoServicePtr svc;
	if(!svc)
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
		int idx = m_index % m_poolsize;
		svc = m_svclist[idx];
		m_index++;
	}
	return svc;
}

int IoServiceManagerImpl::GetServiceCount()
{
	return m_poolsize;
}

esnlib::IoServiceManagerPtr esnlib::CreateIoServiceManager(int poolSize, int svcStackSize)
{
    esnlib::IoServiceManagerPtr iosvcmgr(new IoServiceManagerImpl(poolSize, svcStackSize));
    return iosvcmgr;
}
