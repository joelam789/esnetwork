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

#include <cstring>

#include "StringCodec.h"

using namespace esnlib;

StringCodec::StringCodec()
{
    //ctor
    m_separator = '\n';
    m_maxstrlen = 1024;
}

StringCodec::StringCodec(char separator, int maxstrlen)
{
    //ctor
    m_separator = separator;
    m_maxstrlen = maxstrlen;
}

StringCodec::~StringCodec()
{
    //dtor
}

IoBufferPtr StringCodec::Encode(SessionPtr session, void* data)
{
    IoBufferPtr newtask;
    if(!data) return newtask;

    char* buf = (char*)data;
    int len = strlen(buf);
    newtask = session->GetFreeBuffer(len + 1);
    if(newtask)
    {
        newtask->PutBuf(buf, len);
        newtask->PutByte(m_separator);
        newtask->SetWritePos(0);
    }
    return newtask;
}
void* StringCodec::Decode(SessionPtr session, IoBufferPtr data)
{
    if(!data) return NULL;

    int len = data->size();
    char lastchar = *(data->data() + (len - 1));
    if(lastchar != 0)
    {
        data->size(len + 1);
        data->SetWritePos(len);
        data->PutByte('\0');
        data->SetWritePos(0);
    }
    return data->data();
}

bool StringCodec::Extract(SessionPtr session, IoBufferPtr data, std::vector<IoBufferPtr>& readylist)
{
    IoBufferPtr cache = session->GetReadCache();

    int total = 0;

    while(data->size() - data->GetReadPos() > 0)
    {
        char ch = data->GetByte();
        if(ch == m_separator)
        {
            if(cache->size() > 0)
            {
                IoBufferPtr newtask = session->GetFreeBuffer(cache->size());
                if(!newtask) return false;

                newtask->PutBuf(cache->data(), cache->size());
                readylist.push_back(newtask);
                total++;
                cache->size(0);
                cache->SetReadPos(0);
                cache->SetWritePos(0);
                cache->flag(0);
            }
        }
        else
        {
            cache->size(cache->size() + 1);
            cache->PutByte(ch);

            if(cache->size() >= m_maxstrlen)
            {
                IoBufferPtr newtask = session->GetFreeBuffer(cache->size());
                if(!newtask) return false;

                newtask->PutBuf(cache->data(), cache->size());
                readylist.push_back(newtask);
                total++;
                cache->size(0);
                cache->SetReadPos(0);
                cache->SetWritePos(0);
                cache->flag(0);
            }
        }
    }

    //printf("End decode ...\n");

    if (total > 0 && cache->size() <= 0) return true;

    return false;


}

