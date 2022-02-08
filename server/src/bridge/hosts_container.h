#ifndef HOSTS_CONTAINER_H_
# define HOSTS_CONTAINER_H_

#include "host.h"
#include <unordered_map>
#include <shared_mutex>
#include <memory.h>

class HostsContainer
{

public:

    void		AddHost(Host::Ptr &host);
    void                DeleteHost(Host::Ptr &host);
    Host::Ptr           FindHostByID(const char *id);
    size_t              Size();
    
    template<typename Functor>
    void                Foreach(Functor functor)
    {
      std::lock_guard<std::mutex> lock(mutex);
      auto it = hosts.begin();
      while (it != hosts.end())
      {
	functor(it->second);
	it++;
      }
    }
    
private:

    using Map = std::unordered_map<std::string, Host::Ptr>;

    std::mutex          mutex;
    Map                 hosts;
};

#endif // HOSTS_CONTAINER_H_
