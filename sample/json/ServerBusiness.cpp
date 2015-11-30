#include "ServerBusiness.h"

using namespace esnlib;

ServerBusiness::ServerBusiness()
{
	m_times = 0;
}

int ServerBusiness::GetProcessTimes()
{
	return m_times;
}
void ServerBusiness::UpdateProcessTimes(int times)
{
    boost::unique_lock<boost::mutex> lock(m_datamutex);
	m_times += times;
}
