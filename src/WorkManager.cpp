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
#include <vector>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "WorkManager.h"

using namespace esnlib;

class WorkManagerImpl;

class Worker
{
private:
    boost::thread * m_thread;

    boost::mutex m_mutex;
    boost::condition_variable m_condition;

    WorkManagerImpl* m_mgr;
    int m_state; // -1: invalid, 0: stopping, 1: ready, 2: running
    int m_stacksize;
    std::list<BufferPtr> m_tasks;
    std::list<WorkPtr> m_works;

public:

    Worker(WorkManagerImpl* mgr, int stacksize = 16 * 1024);
    virtual ~Worker();

    virtual int GetState() const;
    virtual void* GetThread() const;

    void SetWorkTask(WorkPtr work, BufferPtr task);

    int Start();

    int DoWork();
    void WaitForNewWork();

    void GoToWork();

    int Stop();
    int Abort();

};

class WorkManagerImpl : public WorkManager
{
private:

    int m_stacksize;

    //int m_maxqueuesize;

    int m_minworkercount;
    int m_maxworkercount;

    std::list<WorkPtr> m_works;
    std::list<BufferPtr> m_tasks;
    std::vector<Worker*> m_workers;
    std::list<Worker*> m_freelist;

    int m_workreq;

    //semaphore m_dispatcher;
    boost::condition_variable m_condition;
    boost::thread * m_dispatcherthread;
    boost::mutex m_dispatchermutex;

    boost::mutex m_taskmutex;
    boost::mutex m_workmutex;
    boost::mutex m_workermutex;

    std::map<int, WorkPtr> m_intworkmap;
    std::map<std::string, WorkPtr> m_strworkmap;

    BufferManagerPtr m_bufmgr;

    int m_state; // -1: invalid, 0: stopping, 1: ready, 2: running

protected:

public:
    WorkManagerImpl(int minworkercount, int maxworkercount, int workstack);
    virtual ~WorkManagerImpl();

    int TakeBack(Worker* worker);
    void TakeBack(BufferPtr buf);

    void WaitForWork();

    int Dispatch();
    int StopDispaching();

    //int start();
    virtual int Stop();
    virtual int Abort();

    virtual int GetState();
    virtual int GetWorkerCount();
    virtual int GetTaskCount();
    virtual int AddWorkTask(WorkPtr work, BufferPtr workdata);

    virtual void SetBufferManager(BufferManagerPtr manager);

    virtual int PutWork(int key, WorkPtr work);
    virtual WorkPtr GetWork(int key);

    virtual int PutWork(const std::string& key, WorkPtr work);
    virtual WorkPtr GetWork(const std::string& key);

};



static void * worker_thread_func( void * ptr )
{
    if(!ptr) return NULL;
    Worker* worker = (Worker*) ptr;
    int ret = worker->Start();
    return (void*) ret;
}

static void * dispatcher_thread_func( void * ptr )
{
    if(!ptr) return NULL;
    WorkManagerImpl* man = (WorkManagerImpl*) ptr;
    int ret = man->Dispatch();
    return (void*) ret;
}


Worker::Worker(WorkManagerImpl* mgr, int stacksize)
{
    m_mgr = mgr;
    m_stacksize = stacksize;
    if(m_stacksize < 0) m_stacksize = 0;

    m_thread = NULL;

    //m_state = -1;
    m_state = 1;

    if(m_stacksize > 0)
    {
        boost::thread::attributes attrs;
        attrs.set_stack_size(stacksize);
        m_thread = new boost::thread(attrs, boost::bind(worker_thread_func, this));
    }
    else
    {
        m_thread = new boost::thread(boost::bind(worker_thread_func, this));
    }


    //m_state = 1;

}

Worker::~Worker()
{
    if(m_state > 0) Abort();

    m_works.clear();
    m_tasks.clear();

    if(m_thread)
    {
        delete m_thread;
        m_thread = NULL;
    }
}

int Worker::GetState() const
{
    return m_state;
}
void* Worker::GetThread() const
{
    return (void*)&m_thread;
}

int Worker::Start()
{
    int ret = 0;

    //if(m_works.size() == 0 || m_tasks.size() == 0) this->Sleep();

    while(m_state > 0)
    {
        WaitForNewWork();
        ret = DoWork();
    }
    return ret;
}

void Worker::SetWorkTask(WorkPtr work, BufferPtr task)
{
    m_works.push_back(work);
    m_tasks.push_back(task);
}

int Worker::DoWork()
{
    if(m_state <= 0) return 0;

    //m_state = 2;

    int ret = 0;

    if(m_works.size() > 0 && m_tasks.size() > 0)
    {
        WorkPtr work = m_works.front();
        BufferPtr task = m_tasks.front();
        ret = work->Run(task);
        while(!m_tasks.empty()) m_tasks.pop_front();
        while(!m_works.empty()) m_works.pop_front();

        if(task->recyclable())
        {
            if(m_mgr) m_mgr->TakeBack(task);
        }
    }

    return ret;
}
void Worker::WaitForNewWork()
{
    boost::unique_lock<boost::mutex> lock(m_mutex);

    if(m_state <= 0) return;

    bool needtakeback = m_state > 1;

    m_state = 1;

    //printf("\nWAIT: %d\n", (int)&m_thread);
    if(m_mgr && needtakeback) m_mgr->TakeBack(this);

    while(m_state == 1) m_condition.wait(lock);
}

void Worker::GoToWork()
{
    boost::unique_lock<boost::mutex> lock(m_mutex);

    if(m_state <= 0) return;
    m_state = 2;

    m_condition.notify_one();
}

int Worker::Stop()
{
    // lock nothing coz care nothing ...

    if(m_state <= 0) return m_state;
    m_state = 0;

    m_condition.notify_one(); // just send out a signal

    if(m_thread) m_thread->join(); // and wait for end

    m_state = -1;
    return m_state;
}

int Worker::Abort()
{
    // lock nothing coz care nothing ...

    if(m_state <= 0) return m_state;
    m_state = -1;

    if(m_thread) m_thread->interrupt(); // force to get out

    //usleep(50 * 1000);
    return m_state;
}



WorkManagerImpl::WorkManagerImpl(int minworkercount, int maxworkercount, int workstack)
{
    m_stacksize = workstack;
    if(m_stacksize < 0) m_stacksize = 0;

    //m_maxqueuesize = maxqueuesize;

    m_minworkercount = minworkercount;
    m_maxworkercount = maxworkercount;

    unsigned cores = boost::thread::hardware_concurrency();

    if (m_minworkercount <= 0) m_minworkercount = cores;
    if (m_minworkercount <= 0) m_minworkercount = 1;

    if (m_maxworkercount <= 0) m_maxworkercount = cores * 2;
    if (m_maxworkercount <= 0) m_maxworkercount = 2;

    m_dispatcherthread = NULL;

    m_workreq = 0;

    //m_state = -1;
    m_state = 1;

    for(int i=1; i<=m_minworkercount; i++)
    {
        Worker* worker = new Worker(this, m_stacksize);
        m_workers.push_back(worker);
        m_freelist.push_back(worker);
    }

    if(m_stacksize > 0)
    {
        boost::thread::attributes attrs;
        attrs.set_stack_size(m_stacksize);
        m_dispatcherthread = new boost::thread(attrs, boost::bind(dispatcher_thread_func, this));
    }
    else
    {
        m_dispatcherthread = new boost::thread(boost::bind(dispatcher_thread_func, this));
    }


}

WorkManagerImpl::~WorkManagerImpl()
{
    Stop();

    m_works.clear();
    m_tasks.clear();
    m_freelist.clear();

    int count = m_workers.size();
    for(int i=0; i<count; i++)
    {
        Worker* worker = m_workers[i];
        if(worker) delete worker;
    }

    m_workers.clear();

    if(m_dispatcherthread)
    {
        delete m_dispatcherthread;
        m_dispatcherthread = NULL;
    }

}

int WorkManagerImpl::PutWork(int key, WorkPtr work)
{
    boost::unique_lock<boost::mutex> lock(m_workmutex);

    std::map<int, WorkPtr>::iterator it = m_intworkmap.find(key);
	if (it != m_intworkmap.end()) m_intworkmap.erase(it);
	m_intworkmap.insert(std::map<int, WorkPtr>::value_type(key, work));
	return m_intworkmap.size();
}
WorkPtr WorkManagerImpl::GetWork(int key)
{
    boost::unique_lock<boost::mutex> lock(m_workmutex);

    WorkPtr work;
    std::map<int, WorkPtr>::iterator it = m_intworkmap.find(key);
	if (it != m_intworkmap.end()) work = it->second;
	return work;
}

int WorkManagerImpl::PutWork(const std::string& key, WorkPtr work)
{
    boost::unique_lock<boost::mutex> lock(m_workmutex);

    std::map<std::string, WorkPtr>::iterator it = m_strworkmap.find(key);
	if (it != m_strworkmap.end()) m_strworkmap.erase(it);
	m_strworkmap.insert(std::map<std::string, WorkPtr>::value_type(key, work));
	return m_strworkmap.size();
}
WorkPtr WorkManagerImpl::GetWork(const std::string& key)
{
    boost::unique_lock<boost::mutex> lock(m_workmutex);

    WorkPtr work;
    std::map<std::string, WorkPtr>::iterator it = m_strworkmap.find(key);
	if (it != m_strworkmap.end()) work = it->second;
	return work;
}


int WorkManagerImpl::StopDispaching()
{
    // lock nothing coz care nothing ...

    if(m_state <= 0) return m_state;
    m_state = 0;

    m_workreq = 1;

    m_condition.notify_one(); // just send out a signal

    if(m_dispatcherthread) m_dispatcherthread->join(); // and wait for end

    m_state = -1;
    return m_state;

}

int WorkManagerImpl::TakeBack(Worker* worker)
{
    if(!worker) return -1;

    int ret = 0;

    if(worker)
    {
        boost::unique_lock<boost::mutex> lock(m_workermutex);
        m_freelist.push_back(worker);
        ret = m_freelist.size();
    }

    //if(ret > 0) m_dispatcher.signal();
    if(ret > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_dispatchermutex);
        m_workreq++;
        m_condition.notify_one();
    }

    return ret;
}

void WorkManagerImpl::TakeBack(BufferPtr buf)
{
    if(m_bufmgr) m_bufmgr->TakeBack(buf);
}

void WorkManagerImpl::WaitForWork()
{
    boost::unique_lock<boost::mutex> lock(m_dispatchermutex);
    while(m_workreq <= 0) m_condition.wait(lock);
    m_workreq--;
    if(m_workreq < 0) m_workreq = 0;
}

int WorkManagerImpl::Dispatch()
{
    while(m_state > 0)
    {
        WaitForWork();

        //std::cout << "Start to dispatch ... " << std::endl;

        Worker* worker = NULL;

        if(m_state > 0)
        {
            boost::unique_lock<boost::mutex> locktasks(m_taskmutex);

            WorkPtr work;
            BufferPtr task;

            if(m_works.size() > 0) work = m_works.front();
            if(m_tasks.size() > 0) task = m_tasks.front();

            if(work)
            {
                if(task)
                {
                    boost::unique_lock<boost::mutex> lockworkers(m_workermutex);
                    if(m_freelist.size() > 0)
                    {
                        worker = m_freelist.front();
                        m_freelist.pop_front();
                    }
                    else
                    {
                        int workercount = m_workers.size();
                        if(workercount < m_maxworkercount || m_maxworkercount <= 0)
                        {
                            Worker* newworker = new Worker(this, m_stacksize);
                            m_workers.push_back(newworker);
                            worker = newworker;
                        }
                    }

                }

                if(worker)
                {
                    worker->SetWorkTask(work, task);
                    m_works.pop_front();
                    m_tasks.pop_front();
                }
            }
        }

        if(worker) worker->GoToWork();

        //m_dispatcher.wait();
        //usleep(50 * 1000);
    }
    return m_state;

}

int WorkManagerImpl::Stop()
{
    if(m_state <= 0) return m_state;
    StopDispaching();
    int count = m_workers.size();
    for(int i=0; i<count; i++)
    {
        Worker* worker = m_workers[i];
        if(worker) worker->Stop();
    }
    m_state = -1;
    return m_state;
}
int WorkManagerImpl::Abort()
{
    if(m_state <= 0) return m_state;
    m_state = -1;
    if(m_dispatcherthread) m_dispatcherthread->interrupt();
    //usleep(50 * 1000);
    int count = m_workers.size();
    for(int i=0; i<count; i++)
    {
        Worker* worker = m_workers[i];
        if(worker) worker->Abort();
    }
    m_state = -1;
    return m_state;
}

int WorkManagerImpl::GetState()
{
    return m_state;
}

int WorkManagerImpl::GetWorkerCount()
{
    return m_workers.size();
}
int WorkManagerImpl::GetTaskCount()
{
    return m_tasks.size();
}

int WorkManagerImpl::AddWorkTask(WorkPtr work, BufferPtr task)
{
    if(m_state <= 0) return m_state;

    int ret = 0;

    if(m_state > 0)
    {
        boost::unique_lock<boost::mutex> locktasks(m_taskmutex);
        //printf("\nBIND: %d - %d\n", (int)task.get(), (int)task.use_count());
        m_works.push_back(work);
        m_tasks.push_back(task);
        //printf("\nBIND: %d - %d\n", (int)task.get(), (int)task.use_count());
        ret = m_tasks.size();
    }

    //if(ret > 0) m_dispatcher.signal();
    if(ret > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_dispatchermutex);
        m_workreq++;
        m_condition.notify_one();
    }

    return ret;

}

void WorkManagerImpl::SetBufferManager(BufferManagerPtr manager)
{
    m_bufmgr = manager;
}

esnlib::WorkManagerPtr esnlib::CreateWorkManager(int minworkercount, int maxworkercount, int workstack)
{
    esnlib::WorkManagerPtr mgr(new WorkManagerImpl(minworkercount, maxworkercount, workstack));
    return mgr;
}

