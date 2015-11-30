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

#include <iostream>

#include "MessageCodec.h"

using namespace esnlib;

MessageCodec::MessageCodec()
{
    m_headersize = 4;
    m_bodysizepos = 0;
    m_bodysizelen = 4;
    m_maxbodysize = 1024;
}

MessageCodec::MessageCodec(int headerSize)
{
    m_headersize = headerSize;
    m_bodysizepos = 0;
    m_bodysizelen = 4;
    m_maxbodysize = 1024;

    if(m_headersize < 1) m_headersize = 1;
    if(m_bodysizelen > m_headersize) m_bodysizelen = m_headersize;
}
MessageCodec::MessageCodec(int headerSize, int bodySizePos)
{
    m_headersize = headerSize;
    m_bodysizepos = bodySizePos;
    m_bodysizelen = 4;
    m_maxbodysize = 1024;

    if(m_headersize < 1) m_headersize = 1;
    if(m_bodysizelen > m_headersize) m_bodysizelen = m_headersize;
    if(m_bodysizepos < 0 || m_bodysizepos > m_headersize - m_bodysizelen) m_bodysizepos = -1;
}
MessageCodec::MessageCodec(int headerSize, int bodySizePos, int bodySizeLength)
{
    m_headersize = headerSize;
    m_bodysizepos = bodySizePos;
    m_bodysizelen = bodySizeLength;
    m_maxbodysize = 1024;

    if(m_headersize < 1) m_headersize = 1;
    if(m_bodysizelen < 0) m_bodysizelen = 0;
    if(m_bodysizelen > 4) m_bodysizelen = 4;
    if(m_bodysizelen > m_headersize) m_bodysizelen = m_headersize;
    if(m_bodysizepos < 0 || m_bodysizepos > m_headersize - m_bodysizelen) m_bodysizepos = -1;
}
MessageCodec::MessageCodec(int headerSize, int bodySizePos, int bodySizeLength, int maxBodySize)
{
    m_headersize = headerSize;
    m_bodysizepos = bodySizePos;
    m_bodysizelen = bodySizeLength;
    m_maxbodysize = maxBodySize;

    if(m_headersize < 1) m_headersize = 1;
    if(m_bodysizelen < 0) m_bodysizelen = 0;
    if(m_bodysizelen > 4) m_bodysizelen = 4;
    if(m_bodysizelen > m_headersize) m_bodysizelen = m_headersize;
    if(m_bodysizepos < 0 || m_bodysizepos > m_headersize - m_bodysizelen) m_bodysizepos = -1;
    if(m_maxbodysize < 0) m_maxbodysize = 0;
}

MessageCodec::~MessageCodec()
{
    //dtor
}


IoBufferPtr MessageCodec::Encode(SessionPtr session, void* data)
{
    IoBufferPtr newtask;
    IoBuffer* buf = (IoBuffer*)data;
    if(buf) newtask = session->GetFreeBuffer(buf->size());
    if(newtask && buf) newtask->PutBuf(buf->data(), buf->size());
    return newtask;
}
void* MessageCodec::Decode(SessionPtr session, IoBufferPtr data)
{
    if(data) return (void*)(data.get());
    else return NULL;
}

bool MessageCodec::Extract(SessionPtr session, IoBufferPtr data, std::vector<IoBufferPtr>& readylist)
{
    //printf("Start decode ...\n");

    IoBufferPtr cache = session->GetReadCache();
    //if(!cache) return false;

    int total = 0;

    if (cache->state() == STATE_WAIT_FOR_BODY)
    {
        int bodylen = cache->flag();
        if (data->size() - data->GetReadPos() >= bodylen)
        {
            //IoTaskPtr newtask(new IoTask(m_headersize + bodylen));
            IoBufferPtr newtask = session->GetFreeBuffer(m_headersize + bodylen);
            if(!newtask) return false;

            newtask->PutBuf(cache->data(), m_headersize);
            newtask->PutBuf(data->data() + data->GetReadPos(), bodylen);
            readylist.push_back(newtask);
            total++;
            data->SetReadPos(data->GetReadPos() + bodylen);
            cache->state(STATE_WAIT_FOR_HEADER);
            cache->SetReadPos(0);
            cache->SetWritePos(0);
            cache->flag(0);
        }

    }

    while(cache->state() == STATE_WAIT_FOR_HEADER
            && data->size() - data->GetReadPos() >= m_headersize)
    {
        if (cache->state() == STATE_WAIT_FOR_HEADER)
        {
            cache->size(m_headersize);
            cache->SetReadPos(0);
            cache->SetWritePos(0);

            if (data->size() - data->GetReadPos() >= m_headersize)
            {
                int bodylen = 0;
                int currentpos = data->GetReadPos();
                cache->PutBuf(data->data() + currentpos, m_headersize);
                if(m_bodysizepos >= 0 && m_bodysizelen > 0)
                {
                    data->SetReadPos(currentpos + m_bodysizepos);
                    if(m_bodysizelen == 1) bodylen = data->GetByte();
                    else if(m_bodysizelen == 2) bodylen = data->GetShort();
                    else bodylen = data->GetInt();
                }

                data->SetReadPos(currentpos + m_headersize);

                //printf("body len: %d ...\n", bodylen);

                if(bodylen > 0)
                {
                    if(bodylen > m_maxbodysize) return true;

                    cache->size(m_headersize + bodylen);
                    if(cache->size() < m_headersize + bodylen) return true;

                    cache->flag(bodylen);
                    cache->state(STATE_WAIT_FOR_BODY);
                }
                else
                {
                    //IoTaskPtr newtask(new IoTask(m_headersize));
                    IoBufferPtr newtask = session->GetFreeBuffer(m_headersize);
                    if(!newtask) return true;

                    newtask->PutBuf(cache->data(), m_headersize);
                    readylist.push_back(newtask);
                    total++;
                    cache->state(STATE_WAIT_FOR_HEADER);
                    cache->SetReadPos(0);
                    cache->SetWritePos(0);
                    cache->flag(0);
                    continue;
                }

            }

        }

        if (cache->state() == STATE_WAIT_FOR_BODY)
        {
            int bodylen = cache->flag();
            if (data->size() - data->GetReadPos() >= bodylen)
            {
                //IoTaskPtr newtask(new IoTask(m_headersize + bodylen));
                IoBufferPtr newtask = session->GetFreeBuffer(m_headersize + bodylen);
                if(!newtask) return false;

                newtask->PutBuf(cache->data(), m_headersize);
                newtask->PutBuf(data->data() + data->GetReadPos(), bodylen);
                readylist.push_back(newtask);
                total++;
                data->SetReadPos(data->GetReadPos() + bodylen);
                cache->state(STATE_WAIT_FOR_HEADER);
                cache->SetReadPos(0);
                cache->SetWritePos(0);
                cache->flag(0);
                continue;
            }

        }

    }

    //printf("End decode ...\n");

    if (total > 0 && data->size() - data->GetReadPos() <= 0) return true;

    if (cache->state() != STATE_WAIT_FOR_HEADER && cache->state() != STATE_WAIT_FOR_BODY)
    {
        std::cout << "MessageCodec::Decode(): Wrong message state found! The session will be closed." << std::endl;
        session->Close();
    }

    return false;
}



