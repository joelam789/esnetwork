#ifndef _ESN_CLI_BUSINESS_H_
#define _ESN_CLI_BUSINESS_H_

#include <string>

#include "Business.h"

namespace esnlib
{

class ClientBusiness : public Business
{
public:

	ClientBusiness();

	void SetLastRequest(int reqact, int reqcount, const std::string& reqstr);
	bool SetLastReply(int replyact, const std::string& replystr);
	bool CheckReply(int replyact, const std::string& replystr);
	const std::string& GetLastRequestStr();
	const std::string& GetReplayStr();
	int GetErrorCount();
	int GetPassCount();

    static bool GetSingleMode();
	static void SetSingleMode(bool value);

	static int GetRequestStringCount();
	static void SetRequestStringCount(int value);

	static std::string GetRandomStr();

protected:

private:

	int m_errors;
	int m_passes;
	int m_lastact;

	int m_reqcount;
	int m_replycount;

	std::string m_laststr;
	std::string m_replystr;

};

}

#endif // _ESN_CLI_BUSINESS_H_
