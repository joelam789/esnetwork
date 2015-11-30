#ifndef _ESN_CHATHANDLER_H_
#define _ESN_CHATHANDLER_H_

#include <set>
#include <vector>
#include <string>

#include "StringHandler.h"

namespace esnlib
{

class ChatHandler : public StringHandler
{
public:
    ChatHandler();
    virtual ~ChatHandler();

    virtual void HandleString(SessionPtr session, const std::string& str);

protected:
private:

    std::set<std::string> m_users;

};

typedef boost::shared_ptr<ChatHandler> ChatHandlerPtr;

}

#endif // _ESN_CHATHANDLER_H_
