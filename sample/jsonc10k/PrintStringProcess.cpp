

#include <string>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include "Session.h"
#include "json.h"

#include "PrintStringProcess.h"

using namespace esnlib;

static boost::mutex g_printtextmutex;

int PrintStringProcess::Handle(IoBufferPtr data)
{
    if(!data) return 0;

	SessionPtr session = data->session();
    if(!session) return 0;

    std::string jsonstr = "";

    char * str = (char*) session->DecodeBuffer(data);
    if(str) jsonstr = str;

    json::Object msg = json::Deserialize(jsonstr);
    std::string text = msg["text"];

    boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

    boost::unique_lock<boost::mutex> lock(g_printtextmutex);

    boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S");
    std::cout.imbue(std::locale(std::cout.getloc(), facet));

    boost::posix_time::ptime current = boost::posix_time::second_clock::local_time();
    std::cout << "[" << current << "]: " << text << std::endl;

    return 0;

}
