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

//#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "IoBuffer.h"

using namespace esnlib;

IoBuffer::IoBuffer()
:m_size(ESN_DEFAULT_IOBUF_SIZE)
,m_max(ESN_DEFAULT_IOBUF_SIZE)
{
    m_data = (char*)malloc(ESN_DEFAULT_IOBUF_SIZE);
    m_flag = 0;
    m_code = 0;
    m_type = 0;
    m_state = 0;
    m_default = 1;
    m_readpos = 0;
    m_writepos = 0;
    m_recyclable = false;

    if(!m_data)
    {
        m_size = 0;
        m_max = m_size;
    }

    //printf("Begin IoTask: %d\n", (int)this);
}

IoBuffer::IoBuffer(int size)
{
    m_size = size;
    if(m_size < 0) m_size = 0;

    if(m_size > ESN_MAX_IOBUF_SIZE)
    {
        m_size = ESN_MAX_IOBUF_SIZE;
        size = ESN_MAX_IOBUF_SIZE;
    }
    else
    {
        int leftsize = m_size % ESN_DEFAULT_IOBUF_SIZE;
        if(leftsize > 0) m_size = m_size + ESN_DEFAULT_IOBUF_SIZE - leftsize;
    }

    m_max = m_size;

    if(m_size > 0) m_data = (char*)malloc(m_size);
    else m_data = NULL;

    m_size = size;
    if(m_size < 0) m_size = 0;

    m_flag = 0;
    m_code = 0;
    m_type = 0;
    m_state = 0;

    m_default = 0;

    m_readpos = 0;
    m_writepos = 0;

    m_recyclable = false;

    if(!m_data)
    {
        m_size = 0;
        m_max = m_size;
    }

    //printf("Begin IoTask: %d\n", (int)this);
}

IoBuffer::IoBuffer(const char* buf, int bufsize)
{
    m_size = bufsize;
    if(m_size < 0) m_size = 0;

    if(m_size > ESN_MAX_IOBUF_SIZE)
    {
        m_size = ESN_MAX_IOBUF_SIZE;
        bufsize = ESN_MAX_IOBUF_SIZE;
    }
    else
    {
        int leftsize = m_size % ESN_DEFAULT_IOBUF_SIZE;
        if(leftsize > 0) m_size = m_size + ESN_DEFAULT_IOBUF_SIZE - leftsize;
    }

    m_max = m_size;

    if(m_size > 0) m_data = (char*)malloc(m_size);
    else m_data = NULL;

    if(m_data && buf) memcpy(m_data, buf, bufsize);

    m_size = bufsize;
    if(m_size < 0) m_size = 0;

    m_flag = 0;
    m_code = 0;
    m_type = 0;
    m_state = 0;

    m_default = 0;

    m_readpos = 0;
    m_writepos = 0;

    m_recyclable = false;

    if(!m_data)
    {
        m_size = 0;
        m_max = m_size;
    }

    //printf("Begin IoTask: %d\n", (int)this);

}

IoBuffer::IoBuffer(const IoBuffer& src)
{
    m_size = src.m_size;
    if(m_size < 0) m_size = 0;

    int leftsize = m_size % ESN_DEFAULT_IOBUF_SIZE;
    if(leftsize > 0) m_size = m_size + ESN_DEFAULT_IOBUF_SIZE - leftsize;

    m_max = m_size;

    if(m_size > 0) m_data = (char*)malloc(m_size);
    else m_data = NULL;

    if(m_data && src.m_data) memcpy(m_data, src.m_data, src.m_size);

    m_size = src.m_size;
    if(m_size < 0) m_size = 0;

    m_flag = src.m_flag;
    m_code = src.m_code;
    m_type = src.m_type;
    m_state = src.m_state;

    m_default = src.m_default;

    m_session = src.m_session;

    m_readpos = 0;
    m_writepos = 0;

    m_recyclable = false;

    if(!m_data)
    {
        m_size = 0;
        m_max = m_size;
    }

}

IoBuffer::~IoBuffer()
{
    if (m_data) free(m_data);
    //printf("End IoTask: %d\n", (int)this);
}

bool IoBuffer::anysize() const
{
    return m_default != 0;
}

const char* IoBuffer::data() const
{
    return m_data;
}

char* IoBuffer::data()
{
    return m_data;
}

int IoBuffer::flag() const
{
    return m_flag;
}
void IoBuffer::flag(int value)
{
    m_flag = value;
}

int IoBuffer::code() const
{
    return m_code;
}
void IoBuffer::code(int value)
{
    m_code = value;
}
int IoBuffer::type() const
{
    return m_type;
}
void IoBuffer::type(int value)
{
    m_type = value;
}
int IoBuffer::state() const
{
    return m_state;
}
void IoBuffer::state(int value)
{
    m_state = value;
}

int IoBuffer::size() const
{
    return m_size;
}

int IoBuffer::size(int value)
{
    if(value < 0) return m_size;

    if(value > ESN_MAX_IOBUF_SIZE) value = ESN_MAX_IOBUF_SIZE;

    if(m_size == value) return m_size;

    if(value > m_max)
    {
        int newsize = value;

        int leftsize = newsize % ESN_DEFAULT_IOBUF_SIZE;
        if(leftsize > 0) newsize = newsize + ESN_DEFAULT_IOBUF_SIZE - leftsize;

        //printf("calling realloc(): %d(%d) => %d(%d) \n", m_size, m_max, value, newsize);

        char* newdata = NULL;
        if(newsize > 0) newdata = (char*)malloc(newsize);

        if(newdata)
        {
            if(newdata && m_data) memcpy(newdata, m_data, m_size);

            if(m_data) free(m_data);

            m_data = newdata;

            m_size = value;
            m_max = newsize;
        }
    }
    else
    {
        m_size = value;
    }

    return m_size;
}


SessionPtr IoBuffer::session()
{
    return m_session.lock();
}
void IoBuffer::session(SessionPtr value)
{
    //if(m_session.expired()) m_session = value;
    m_session = value;
}

int IoBuffer::GetReadPos()
{
    return m_readpos;
}
void IoBuffer::SetReadPos(int pos)
{
    m_readpos = pos;
}
int IoBuffer::GetWritePos()
{
    return m_writepos;
}
void IoBuffer::SetWritePos(int pos)
{
    m_writepos = pos;
}

char IoBuffer::GetByte()
{
    if(!m_data) return '\0';
    int datasize = sizeof(char);
    if(m_readpos + datasize > m_size) return '\0';
    else
    {
        char dst = *(m_data + m_readpos);
        m_readpos += datasize;
        return dst;
    }
}
short IoBuffer::GetShort()
{
    if(!m_data) return '\0';
    int datasize = sizeof(short);
    if(m_readpos + datasize > m_size) return '\0';
    else
    {
        short dst = 0;
        memcpy((void*)(&dst), (void*)(m_data + m_readpos), datasize);
        m_readpos += datasize;
        return dst;
    }
}
int IoBuffer::GetInt()
{
    if(!m_data) return 0;
    int datasize = sizeof(int);
    if(m_readpos + datasize > m_size) return 0;
    else
    {
        int dst = 0;
        memcpy((void*)(&dst), (void*)(m_data + m_readpos), datasize);
        m_readpos += datasize;
        return dst;
    }
}
double IoBuffer::GetDouble()
{
    if(!m_data) return 0;
    int datasize = sizeof(double);
    if(m_readpos + datasize > m_size) return 0;
    else
    {
        double dst = 0;
        memcpy((void*)(&dst), (void*)(m_data + m_readpos), datasize);
        m_readpos += datasize;
        return dst;
    }
}
int IoBuffer::GetBuf(char* buf, int len)
{
    if(!m_data || !buf) return 0;

    int datasize = len;
    if(m_readpos + datasize > m_size) datasize = m_size - m_readpos;

    if(datasize <= 0) return 0;
    else
    {
        char* dst = buf;
        memcpy((void*)(dst), (void*)(m_data + m_readpos), datasize);
        m_readpos += datasize;
        return datasize;
    }
}
std::string IoBuffer::GetStr(int len)
{
    std::string s = "";

    if(!m_data) return s;

    int datasize = len;
    if(m_readpos + datasize > m_size) datasize = m_size - m_readpos;

    if(datasize <= 0) return s;
    else
    {
        char dst[datasize+1];
        memcpy((void*)(&(dst[0])), (void*)(m_data + m_readpos), datasize);
        dst[datasize] = '\0';
        m_readpos += datasize;
        s = dst;
        return s;
    }
}

void IoBuffer::PutByte(char value)
{
    if(!m_data) return;
    int datasize = sizeof(char);
    if(m_writepos + datasize > m_size) return;
    else
    {
        m_data[m_writepos] = value;
        m_writepos += datasize;
    }
}
void IoBuffer::PutShort(int value)
{
    if(!m_data) return;
    int datasize = sizeof(short);
    if(m_writepos + datasize > m_size) return;
    else
    {
        short val = value;
        char* dst = &(m_data[m_writepos]);
        memcpy((void*)(dst), (void*)(&val), datasize);
        m_writepos += datasize;
    }
}
void IoBuffer::PutInt(int value)
{
    if(!m_data) return;
    int datasize = sizeof(int);
    if(m_writepos + datasize > m_size) return;
    else
    {
        char* dst = &(m_data[m_writepos]);
        memcpy((void*)(dst), (void*)(&value), datasize);
        m_writepos += datasize;
    }
}
void IoBuffer::PutDouble(double value)
{
    if(!m_data) return;
    int datasize = sizeof(double);
    if(m_writepos + datasize > m_size) return;
    else
    {
        char* dst = &(m_data[m_writepos]);
        memcpy((void*)(dst), (void*)(&value), datasize);
        m_writepos += datasize;
    }
}
void IoBuffer::PutBuf(char* buf, int len)
{
    if(!m_data || !buf) return;

    int datasize = len;
    if(m_writepos + datasize > m_size) return;
    else
    {
        char* dst = &(m_data[m_writepos]);
        memcpy((void*)(dst), (void*)(buf), datasize);
        m_writepos += datasize;
    }
}
void IoBuffer::PutStr(const std::string& value)
{
    if(!m_data) return;
    int datasize = value.length();
    if(m_writepos + datasize > m_size) return;
    else
    {
        char* dst = &(m_data[m_writepos]);
        memcpy((void*)(dst), (void*)(value.data()), datasize);
        m_writepos += datasize;
    }
}




