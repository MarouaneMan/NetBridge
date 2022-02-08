#ifndef HOST_H_
# define HOST_H_

#include <string>
#include <set>
#include <memory>
#include <mutex>
#include <algorithm>
#include "sgxlib/net/socket_interface.h"

class Host
{

public:
  
    using Ptr = std::shared_ptr<Host>;
    
    Host(SGX::Net::Socket::TCP::Interface::Ptr &);

    bool                                ReadMessage(size_t);
    bool                                WriteMessage(const char *, size_t);
    std::string                         GetRemoteIP();
    void                                AddRelationShip(const std::string &);
    void                                DeleteRelationShip(const std::string &);
    size_t                              RelationShipsSize();
    
    std::string                             HostID;
    std::string                             Hostname;
    std::string                             LocalIP;
    std::vector<char>                       Buffer;
    uint32_t                                ReadSomeSize;

    template<class Functor>
    void                                    ForeachRelationShip(Functor func)
    {
      std::lock_guard<std::mutex> lock(relationships_mutex);
      std::for_each(relationships.begin(), relationships.end(), func);
    }
    
private:
    
    std::set<std::string>                   relationships;
    SGX::Net::Socket::TCP::Interface::Ptr   conn;
    std::mutex                              relationships_mutex;
    std::string			                    remoteIP;
};

#endif // HOST_H_
