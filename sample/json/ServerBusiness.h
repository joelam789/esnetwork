#ifndef _ESN_SVR_BUSINESS_H_
#define _ESN_SVR_BUSINESS_H_

#include <boost/thread/mutex.hpp>

#include "Business.h"

namespace esnlib
{

class ServerBusiness : public Business
{
public:

	ServerBusiness();

    int GetProcessTimes();
	void UpdateProcessTimes(int times);

protected:

private:

    boost::mutex m_datamutex;

	int m_times;

};

}

#endif // _ESN_SVR_BUSINESS_H_
