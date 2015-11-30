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

#include <list>
#include <sstream>

#include <boost/thread/mutex.hpp>

#include "LogManager.h"

#include "IoBufferManager.h"

using namespace esnlib;

IoBufferManager::IoBufferManager()
{
    //ctor
}

IoBufferManager::~IoBufferManager()
{
    //dtor
}

class IoBufferManagerImpl : public IoBufferManager
{
public:
    IoBufferManagerImpl();
    IoBufferManagerImpl(int itemcount, int bufsize);
    virtual ~IoBufferManagerImpl();

    virtual BufferPtr GetFreeBuffer();
    virtual void TakeBack(BufferPtr buf);

    virtual IoBufferPtr GetFreeBuffer(int bufsize);
    virtual void TakeBack(IoBufferPtr buf);

protected:
private:

    std::list<IoBufferPtr> m_items;
    boost::mutex m_mutex;

    int m_bufsize;

};

IoBufferManagerImpl::IoBufferManagerImpl()
{
    //ctor
    m_bufsize = ESN_DEFAULT_IOBUF_SIZE;
}

IoBufferManagerImpl::IoBufferManagerImpl(int itemcount, int bufsize)
{
    m_bufsize = bufsize;
    if(m_bufsize <= 0) m_bufsize = ESN_DEFAULT_IOBUF_SIZE;

    for(int i=0; i<itemcount; i++)
    {
        IoBufferPtr task(new IoBuffer(m_bufsize));
        m_items.push_back(task);
    }
}

IoBufferManagerImpl::~IoBufferManagerImpl()
{
    m_items.clear();
}

IoBufferPtr IoBufferManagerImpl::GetFreeBuffer(int bufsize)
{
    IoBufferPtr buf;
    if(!buf)
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        if(m_items.size() > 0)
        {
            IoBufferPtr item = m_items.front();
            if(item)
            {
                item->size(bufsize);
                item->SetReadPos(0);
                item->SetWritePos(0);
                item->recyclable(true);
                //printf("Found IoTask: %d\n", (int)(item.get()));
            }
            if(item && item->size() >= bufsize)
            {
                buf = item;
                m_items.pop_front();
            }
        }
        else
        {
            IoBufferPtr newbuf(new IoBuffer(bufsize));
            newbuf->recyclable(true);

            if(newbuf && newbuf->size() >= bufsize) buf = newbuf;
        }
    }

    //if(!buf) std::cout << "Fail to get free buffer with size " << bufsize << std::endl;
    if(!buf)
    {
        std::stringstream ss;
        ss << "Fail to get free buffer with size " << bufsize ;
        LogManager::Warning(ss.str());
    }

    return buf;
}
void IoBufferManagerImpl::TakeBack(IoBufferPtr buf)
{
    IoBufferPtr item = buf;
    if(item)
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        item->recyclable(true);
        m_items.push_back(item);
        //printf("Take back IoTask: %d\n", (int)(task.get()));
    }
}

BufferPtr IoBufferManagerImpl::GetFreeBuffer()
{
    return GetFreeBuffer(m_bufsize);
}
void IoBufferManagerImpl::TakeBack(BufferPtr buf)
{
    IoBufferPtr newtask =  *((IoBufferPtr*)(&buf));
    TakeBack(newtask);
}

esnlib::IoBufferManagerPtr esnlib::CreateIoBufferManager(int itemcount, int bufsize)
{
    esnlib::IoBufferManagerPtr mgr(new IoBufferManagerImpl(itemcount, bufsize));
    return mgr;
}

