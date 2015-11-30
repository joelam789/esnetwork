/*
-----------------------------------------------------------------------------
This source file is part of "clay" library.
It is licensed under the terms of the MIT license.
For the latest info, see http://libclay.sourceforge.net

Copyright (c) 2010-2013 Lin Jia Jun (Joe Lam)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "clay.h"

#include <ctime>

#include <list>
#include <fstream>
#include <iostream>

#define _CLAY_LOG_FILE_  "clay.log"

using namespace clay::lib;
using namespace clay::core;

/* get system time as a simple string */
std::string clay::lib::GetTimeStr()
{
	std::string rsl;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[20];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

    rsl = buffer;

    return rsl;
}

/* write log */
void clay::lib::Log(const char* sMsg)
{
    std::ofstream logfile(_CLAY_LOG_FILE_, std::ios::out | std::ios::app);
	if (!logfile.fail())
	{
		logfile << "[" << clay::lib::GetTimeStr() <<  "]: " << sMsg << "\n";
		logfile.close();
	}
}

int clay::lib::GetSpCount()
{
    return ClayServiceAgent::GetAgent()->GetSpCount();
}

clay::core::ClayServiceAgent* clay::lib::GetAgent()
{
    return ClayServiceAgent::GetAgent();
}

void clay::lib::SetAgent(void* pAgent)
{
    return ClayServiceAgent::GetAgent()->SetExternalAgent((clay::core::ClayServiceAgent*)pAgent);
}

int clay::lib::RegisterSp(clay::core::ClayServiceProvider* pServiceProvider)
{
    return ClayServiceAgent::GetAgent()->RegSp(pServiceProvider);
}

void clay::lib::Clear()
{
    ClayServiceAgent::GetAgent()->Clear();
}

clay::core::ClayServiceProvider* clay::lib::GetProvider(int iSpCode)
{
    return ClayServiceAgent::GetAgent()->GetSp(iSpCode);
}

clay::core::ClayServiceProvider* clay::lib::GetProvider(const std::string& sSpName)
{
    return ClayServiceAgent::GetAgent()->GetSp(sSpName);
}

bool clay::lib::ProviderExists(int iSpCode)
{
    return ClayServiceAgent::GetAgent()->SpExists(iSpCode);
}

bool clay::lib::ProviderExists(const std::string& sSpName)
{
    return ClayServiceAgent::GetAgent()->SpExists(sSpName);
}

bool clay::lib::ServiceExists(int iSpCode, int iSvcCode)
{
    return ClayServiceAgent::GetAgent()->SvcExists(iSpCode, iSvcCode);
}
bool clay::lib::ServiceExists(int iSpCode, const std::string& sSvcName)
{
    return ClayServiceAgent::GetAgent()->SvcExists(iSpCode, sSvcName);
}
bool clay::lib::ServiceExists(const std::string& sSpName, int iSvcCode)
{
    return ClayServiceAgent::GetAgent()->SvcExists(sSpName, iSvcCode);
}
bool clay::lib::ServiceExists(const std::string& sSpName, const std::string& sSvcName)
{
    return ClayServiceAgent::GetAgent()->SvcExists(sSpName, sSvcName);
}

int clay::lib::CallService(int iSpCode, int iSvcCode, void* pParam)
{
    return ClayServiceAgent::GetAgent()->CallService(iSpCode, iSvcCode, pParam);
}
int clay::lib::CallService(int iSpCode, const std::string& sSvcName, void* pParam)
{
    return ClayServiceAgent::GetAgent()->CallService(iSpCode, sSvcName, pParam);
}
int clay::lib::CallService(const std::string& sSpName, int iSvcCode, void* pParam)
{
    return ClayServiceAgent::GetAgent()->CallService(sSpName, iSvcCode, pParam);
}
int clay::lib::CallService(const std::string& sSpName, const std::string& sSvcName, void* pParam)
{
    return ClayServiceAgent::GetAgent()->CallService(sSpName, sSvcName, pParam);
}

/*------------------ ClayServiceProvider ---------------------*/

static const char* CLAY_CUSTOM_SVC_PROVIDER_VERSION = "Custom Base ServiceProvider v1.0.0";
static const char* CLAY_CUSTOM_SVC_GET_VERSION = "GetVersion";

ClayServiceProvider::ClayServiceProvider(int iCode)
{
    m_code = iCode;
    m_name = "";
}

ClayServiceProvider::ClayServiceProvider(const std::string& sName)
{
    m_code = 0;
    m_name = sName;
}

ClayServiceProvider::ClayServiceProvider(int iCode, const std::string& sName)
{
    m_code = iCode;
    m_name = sName;
}

ClayServiceProvider::~ClayServiceProvider()
{
    ClearServices();
}

int ClayServiceProvider::GetCode()
{
    return m_code;
}

std::string ClayServiceProvider::GetName()
{
    return m_name;
}

void ClayServiceProvider::ClearServices()
{
    m_svcintmap.clear();
    m_svcstrmap.clear();
}

int ClayServiceProvider::GetServiceCount()
{
    return m_svcintmap.size() + m_svcstrmap.size();
}

int ClayServiceProvider::RegisterService(int iSvcCode, CLAY_SVC_CLASS_FUNC pSvcFunc)
{
    if (pSvcFunc==NULL) return -1;
    ClayServiceIntMap::iterator it;
	it = m_svcintmap.find(iSvcCode);
	if (it != m_svcintmap.end())
	{
	    return 0;
	}
	else
	{
	    m_svcintmap.insert(ClayServiceIntMap::value_type(iSvcCode, pSvcFunc));
        return 1;
	}

}

int ClayServiceProvider::RegisterService(const std::string& sSvcName, CLAY_SVC_CLASS_FUNC pSvcFunc)
{
    if (pSvcFunc==NULL) return -1;
    ClayServiceStrMap::iterator it;
	it = m_svcstrmap.find(sSvcName);
	if (it != m_svcstrmap.end())
	{
	    return 0;
	}
	else
	{
	    m_svcstrmap.insert(ClayServiceStrMap::value_type(sSvcName, pSvcFunc));
        return 1;
	}

}

int ClayServiceProvider::UnregisterService(int iSvcCode)
{
    ClayServiceIntMap::iterator it;
	it = m_svcintmap.find(iSvcCode);
	if (it != m_svcintmap.end())
	{
        m_svcintmap.erase(it);
        return 1;
	}
	else
	{
		return 0;
	}
}

int ClayServiceProvider::UnregisterService(const std::string& sSvcName)
{
    ClayServiceStrMap::iterator it;
	it = m_svcstrmap.find(sSvcName);
	if (it != m_svcstrmap.end())
	{
        m_svcstrmap.erase(it);
        return 1;
	}
	else
	{
		return 0;
	}
}


CLAY_SVC_CLASS_FUNC ClayServiceProvider::GetService(int iSvcCode)
{
    ClayServiceIntMap::iterator it = m_svcintmap.find(iSvcCode);

    if (it != m_svcintmap.end())
	{
        CLAY_SVC_CLASS_FUNC svc = (CLAY_SVC_CLASS_FUNC)(it->second);
        return svc;
	}

	return NULL;
}
CLAY_SVC_CLASS_FUNC ClayServiceProvider::GetService(const std::string& sSvcName)
{
    ClayServiceStrMap::iterator it = m_svcstrmap.find(sSvcName);

    if (it != m_svcstrmap.end())
	{
        CLAY_SVC_CLASS_FUNC svc = (CLAY_SVC_CLASS_FUNC)(it->second);
        return svc;
	}

	return NULL;
}

int ClayServiceProvider::DoService(int iSvcCode, void* pParam)
{
    ClayServiceIntMap::iterator it = m_svcintmap.find(iSvcCode);

    if (it != m_svcintmap.end())
	{
        CLAY_SVC_CLASS_FUNC svc = (CLAY_SVC_CLASS_FUNC)(it->second);
        return (this->*svc)(pParam);
	}
	else
	{
		std::cout << "ServiceProvider::DoService: Can not find Service " <<  m_name << "(" << m_code << ") - " << iSvcCode << std::endl;
		//throw new std::exception();
		return 0;
	}

}

int ClayServiceProvider::DoService(const std::string& sSvcName, void* pParam)
{
    ClayServiceStrMap::iterator it = m_svcstrmap.find(sSvcName);

    if (it != m_svcstrmap.end())
	{
        CLAY_SVC_CLASS_FUNC svc = (CLAY_SVC_CLASS_FUNC)(it->second);
        return (this->*svc)(pParam);
	}
	else
	{
		std::cout << "ServiceProvider::DoService: Can not find Service " <<  m_name << "(" << m_code << ") - " << sSvcName << std::endl;
		//throw new std::exception();
		return 0;
	}

}

bool ClayServiceProvider::ServiceExists(int iSvcCode)
{
    return m_svcintmap.find(iSvcCode) != m_svcintmap.end();
}

bool ClayServiceProvider::ServiceExists(const std::string& sSvcName)
{
    return m_svcstrmap.find(sSvcName) != m_svcstrmap.end();
}

void ClayServiceProvider::SetupServices()
{
    RegisterService(CLAY_CUSTOM_SVC_GET_VERSION, &ClayServiceProvider::GetVersion);
}

int ClayServiceProvider::GetVersion(void* pParam)
{
    return (int)CLAY_CUSTOM_SVC_PROVIDER_VERSION;
}


/*------------------ ClayServiceAgent ---------------------*/

int ClayServiceAgent::RegSp(ClayServiceProvider* pServiceProvider)
{
    if (pServiceProvider==NULL) return -1;

    int result = 0;

    std::string sSpName = pServiceProvider->GetName();
    if(sSpName.length() > 0)
    {
        ClayServiceProviderStrMap::iterator it;
        it = m_spstrmap.find(sSpName);
        if (it == m_spstrmap.end())
        {
            m_spstrmap.insert(ClayServiceProviderStrMap::value_type(sSpName, pServiceProvider));
            result++;
        }
    }

    int iSpCode = pServiceProvider->GetCode();
    if(iSpCode != 0)
    {
        ClayServiceProviderIntMap::iterator it;
        it = m_spintmap.find(iSpCode);
        if (it == m_spintmap.end())
        {
            m_spintmap.insert(ClayServiceProviderIntMap::value_type(iSpCode, pServiceProvider));
            result++;
        }
    }

    return result;

}

int ClayServiceAgent::UnregSp(int iSpCode)
{
    ClayServiceProviderIntMap::iterator it;
	it = m_spintmap.find(iSpCode);
	if (it != m_spintmap.end())
	{
	    ClayServiceProvider* sp = it->second;
        m_spintmap.erase(it);
        if (sp)
        {
            std::string sSpName = sp->GetName();
            ClayServiceProviderStrMap::iterator itx;
            itx = m_spstrmap.find(sSpName);
            if(itx == m_spstrmap.end()) delete sp;
        }
        return 1;
	}
	else
	{
		std::cout << "ServiceAgent::UnRegSp: Can not find ServiceProvider " <<  iSpCode << std::endl;
		return 0;
	}

}

int ClayServiceAgent::UnregSp(const std::string& sSpName)
{
    ClayServiceProviderStrMap::iterator it;
	it = m_spstrmap.find(sSpName);
	if (it != m_spstrmap.end())
	{
	    ClayServiceProvider* sp = it->second;
        m_spstrmap.erase(it);
        if (sp)
        {
            int iSpCode = sp->GetCode();
            ClayServiceProviderIntMap::iterator itx;
            itx = m_spintmap.find(iSpCode);
            if(itx == m_spintmap.end()) delete sp;
        }
        return 1;
	}
	else
	{
		std::cout << "ServiceAgent::UnRegSp: Can not find ServiceProvider " <<  sSpName << std::endl;
		return 0;
	}

}

ClayServiceProvider* ClayServiceAgent::GetSp(int iSpCode)
{
    ClayServiceProviderIntMap::iterator it;
	it = m_spintmap.find(iSpCode);
	if (it != m_spintmap.end())
	{
	    ClayServiceProvider* sp = it->second;
        return sp;
	}
	else
	{
		std::cout << "ServiceAgent::GetSp: Can not find ServiceProvider " <<  iSpCode << std::endl;
		return NULL;
	}
}
ClayServiceProvider* ClayServiceAgent::GetSp(const std::string& sSpName)
{
    ClayServiceProviderStrMap::iterator it;
	it = m_spstrmap.find(sSpName);
	if (it != m_spstrmap.end())
	{
	    ClayServiceProvider* sp = it->second;
        return sp;
	}
	else
	{
		std::cout << "ServiceAgent::GetSp: Can not find ServiceProvider " <<  sSpName << std::endl;
		return NULL;
	}
}

int ClayServiceAgent::GetSpCount()
{
    int result = 0;

    std::list<ClayServiceProvider*> splist;

    ClayServiceProviderIntMap::iterator itx;
    for(itx = m_spintmap.begin(); itx != m_spintmap.end(); itx++)
    {
        ClayServiceProvider* sp = itx->second;
        splist.push_back(sp);
    }

    ClayServiceProviderStrMap::iterator its;
    for(its = m_spstrmap.begin(); its != m_spstrmap.end(); its++)
    {
        ClayServiceProvider* sp = its->second;
        splist.push_back(sp);
    }

    splist.unique();

    result = splist.size();

    return result;
}

void ClayServiceAgent::Clear()
{
    // free all

	std::list<ClayServiceProvider*> splist;

    ClayServiceProviderIntMap::iterator itx;
    for(itx = m_spintmap.begin(); itx != m_spintmap.end(); itx++)
    {
        ClayServiceProvider* sp = itx->second;
        splist.push_back(sp);
    }

    ClayServiceProviderStrMap::iterator its;
    for(its = m_spstrmap.begin(); its != m_spstrmap.end(); its++)
    {
        ClayServiceProvider* sp = its->second;
        splist.push_back(sp);
    }

    m_spintmap.clear();
    m_spstrmap.clear();

    splist.unique();

    while(!splist.empty())
    {
        ClayServiceProvider* sp = splist.back();
        if(sp) delete sp;
        splist.pop_back();
    }

}

bool ClayServiceAgent::SpExists(int iSpCode)
{
    return m_spintmap.find(iSpCode) != m_spintmap.end();
}
bool ClayServiceAgent::SpExists(const std::string& sSpName)
{
    return m_spstrmap.find(sSpName) != m_spstrmap.end();
}

bool ClayServiceAgent::SvcExists(int iSpCode, int iSvcCode)
{
    ClayServiceProviderIntMap::iterator it = m_spintmap.find(iSpCode);
    if (it != m_spintmap.end()) return it->second->ServiceExists(iSvcCode);
	else return false;
}
bool ClayServiceAgent::SvcExists(int iSpCode, const std::string& sSvcName)
{
    ClayServiceProviderIntMap::iterator it = m_spintmap.find(iSpCode);
    if (it != m_spintmap.end()) return it->second->ServiceExists(sSvcName);
	else return false;
}
bool ClayServiceAgent::SvcExists(const std::string& sSpName, int iSvcCode)
{
    ClayServiceProviderStrMap::iterator it = m_spstrmap.find(sSpName);
    if (it != m_spstrmap.end()) return it->second->ServiceExists(iSvcCode);
	else return false;
}
bool ClayServiceAgent::SvcExists(const std::string& sSpName, const std::string& sSvcName)
{
    ClayServiceProviderStrMap::iterator it = m_spstrmap.find(sSpName);
    if (it != m_spstrmap.end()) return it->second->ServiceExists(sSvcName);
	else return false;
}

int ClayServiceAgent::CallService(int iSpCode, int iSvcCode, void* pParam)
{
    ClayServiceProviderIntMap::iterator it = m_spintmap.find(iSpCode);
    if (it != m_spintmap.end()) return it->second->DoService(iSvcCode, pParam);
	else std::cout << "ServiceAgent::CallService: Can not find ServiceProvider " <<  iSpCode << std::endl;
	return 0;
}
int ClayServiceAgent::CallService(int iSpCode, const std::string& sSvcName, void* pParam)
{
    ClayServiceProviderIntMap::iterator it = m_spintmap.find(iSpCode);
    if (it != m_spintmap.end()) return it->second->DoService(sSvcName, pParam);
	else std::cout << "ServiceAgent::CallService: Can not find ServiceProvider " <<  iSpCode << std::endl;
	return 0;
}
int ClayServiceAgent::CallService(const std::string& sSpName, int iSvcCode, void* pParam)
{
    ClayServiceProviderStrMap::iterator it = m_spstrmap.find(sSpName);
    if (it != m_spstrmap.end()) return it->second->DoService(iSvcCode, pParam);
	else std::cout << "ServiceAgent::CallService: Can not find ServiceProvider " <<  sSpName << std::endl;
	return 0;
}
int ClayServiceAgent::CallService(const std::string& sSpName, const std::string& sSvcName, void* pParam)
{
    ClayServiceProviderStrMap::iterator it = m_spstrmap.find(sSpName);
    if (it != m_spstrmap.end()) return it->second->DoService(sSvcName, pParam);
	else std::cout << "ServiceAgent::CallService: Can not find ServiceProvider " <<  sSpName << std::endl;
	return 0;
}


void ClayServiceAgent::SetExternalAgent(ClayServiceAgent* pAgent)
{
    m_external = pAgent;
}

ClayServiceAgent::ClayServiceAgent()
{
    m_external = NULL;
}

ClayServiceAgent::~ClayServiceAgent()
{
    Clear();
}
