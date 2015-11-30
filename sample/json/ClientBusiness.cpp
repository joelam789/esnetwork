#include <cstdlib>

#include <string>

#include <boost/algorithm/string.hpp>

#include "StringReplyProcess.h"
#include "ClientBusiness.h"

using namespace esnlib;

static const char g_alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

static int g_maxcharcount = sizeof(g_alphanum) - 1;
static int g_single_flag = 1;
static int g_string_count = 5;

static char GetRandomChar()
{
    return g_alphanum[rand() % g_maxcharcount];
}

ClientBusiness::ClientBusiness()
{
	m_errors = 0;
	m_passes = 0;
	m_lastact = 0;
	m_laststr = "";
}

bool ClientBusiness::GetSingleMode()
{
    return g_single_flag > 0;
}
void ClientBusiness::SetSingleMode(bool value)
{
    if(value) g_single_flag = 1;
    else g_single_flag = 0;
}

int ClientBusiness::GetRequestStringCount()
{
    return g_string_count;
}
void ClientBusiness::SetRequestStringCount(int value)
{
    g_string_count = value;
    if(g_string_count <= 0) g_string_count = 1;
}

std::string ClientBusiness::GetRandomStr()
{
	int strlen = ( rand() % (g_maxcharcount >> 1) ) + 1;

	std::string str(strlen, '\0');
    for(int i = 0; i < strlen; ++i) str[i] = GetRandomChar();

	return str;
}

void ClientBusiness::SetLastRequest(int reqact, int reqcount, const std::string& reqstr)
{
	m_lastact = reqact;
	m_laststr = reqstr;
	m_reqcount = reqcount;

	m_replycount = 0;
	m_replystr = "";
}

bool ClientBusiness::SetLastReply(int replyact, const std::string& replystr)
{
    if(m_lastact == replyact)
    {
        if(m_replycount < m_reqcount) m_replystr = m_replystr + replystr;
        m_replycount++;
    }

    return m_replycount >= m_reqcount;
}

bool ClientBusiness::CheckReply(int replyact, const std::string& replystr)
{
	bool result = false;

	if(m_lastact == replyact)
	{
		std::string text = m_laststr;
		switch (m_lastact)
		{
			case StringReplyProcess::ACT_UPPER:
				boost::to_upper(text);
				break;

			case StringReplyProcess::ACT_LOWER:
				boost::to_lower(text);
				break;

			default:
				break;
		}
		if(text == replystr)
		{
			result = true;
		}
	}

	if(result) m_passes++;
	else m_errors++;

	return result;
}
const std::string& ClientBusiness::GetLastRequestStr()
{
    return m_laststr;
}
const std::string& ClientBusiness::GetReplayStr()
{
    return m_replystr;
}
int ClientBusiness::GetErrorCount()
{
	return m_errors;
}
int ClientBusiness::GetPassCount()
{
	return m_passes;
}


