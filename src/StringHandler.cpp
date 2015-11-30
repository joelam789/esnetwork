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

#include <ctime>
//#include <cstdio>

#include <vector>
#include <sstream>

#include "LogManager.h"

#include "StringHandler.h"

using namespace esnlib;

StringHandler::StringHandler()
{
    //ctor
}

StringHandler::~StringHandler()
{
    //dtor
}

int StringHandler::SplitStr(const std::string& src, const std::string& delims, std::vector<std::string>& strs)
{
    strs.clear();

    int iLen = delims.size();
    int iLastPos = 0, iIndex = -1;
    while (-1 != (iIndex = src.find(delims,iLastPos)))
    {
        strs.push_back(src.substr(iLastPos, iIndex - iLastPos));
        iLastPos = iIndex + iLen;
    }

    std::string sLastString = src.substr(iLastPos);

    //if (!sLastString.empty())
    strs.push_back(sLastString);

    return strs.size();
}

std::string StringHandler::TrimStr(std::string const& source, char const* spacechars)
{
    std::string result(source);
    std::string::size_type index = result.find_last_not_of(spacechars);
    if(index != std::string::npos) result.erase(++index);
    index = result.find_first_not_of(spacechars);
    if(index != std::string::npos) result.erase(0, index);
    else result.erase();
    return result;
}

std::string StringHandler::GetDateTimeStr()
{
	std::string rsl;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[20];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
    //strftime(buffer, 20, "%H:%M:%S", timeinfo);

    rsl = buffer;

    return rsl;
}

std::string StringHandler::GetTimeStr()
{
	std::string rsl;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[20];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    //strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
    strftime(buffer, 20, "%H:%M:%S", timeinfo);

    rsl = buffer;

    return rsl;
}

int StringHandler::OnRead(SessionPtr session, IoBufferPtr data)
{
    std::string str = (char*) session->DecodeBuffer(data);
    HandleString(session, str);
    return 0;
}
int StringHandler::OnWrite(SessionPtr session, IoBufferPtr data)
{
    return 0;
}

void StringHandler::HandleString(SessionPtr session, const std::string& str)
{
    return;
}

void StringHandler::OnConnect(SessionPtr session)
{
    //printf("Network - Connect Event : %d \n", session->GetId());

    std::stringstream ss;
    ss << "Network - Connect Event : " << session->GetId();
    LogManager::Info(ss.str());
}
void StringHandler::OnDisconnect(SessionPtr session)
{
    //printf("Network - Disconnect Event : %d \n", session->GetId());

    std::stringstream ss;
    ss << "Network - Disconnect Event : " << session->GetId();
    LogManager::Info(ss.str());
}

void StringHandler::OnIdle(SessionPtr session, int idleType)
{
    std::stringstream ss;
    switch (idleType)
    {
        case 0: ss << "Network - Both Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
        case 1: ss << "Network - Receiving Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
        case 2: ss << "Network - Sending Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
        default: ss << "Network Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
    }
}

void StringHandler::OnError(SessionPtr session, int errorType, int errorCode, const std::string& errorMsg)
{
    std::stringstream ss;
    switch (errorType)
    {
        case -1: ss << "Network - Listening Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        case  0: ss << "Network - Connection Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        case  1: ss << "Network - Receiving Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        case  2: ss << "Network - Sending Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        default: ss << "Network Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
    }
    session->Close();
}

