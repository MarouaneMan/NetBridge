#ifndef HOSTS_CONTAINER_H_
# define HOSTS_CONTAINER_H_

#include "host.h"
#include <shared_mutex>
#include <vector>
#include <algorithm>

class HostsContainer
{

public:

    HostsContainer();
    ~HostsContainer();

    void                        AddHost(Host::Ptr);
    void                        DeleteHost(const QString &);
    Host::Ptr                   FindHostByID(const QString &);
    bool                        HostIDExists(const QString &);
    size_t                      Size();
    Host::Ptr                   Back();

    template<typename Functor>
    void                        ForeachReadLock(Functor func)
    {
        // Lock hosts for read
        std::shared_lock<std::shared_mutex> lock(mutex);
        std::for_each(hosts.begin(), hosts.end(), func);
    }
    
    template<typename Functor>
    void                        ForeachWriteLock(Functor func)
    {
        // Lock hosts for read
        std::lock_guard<std::shared_mutex> lock(mutex);
        std::for_each(hosts.begin(), hosts.end(), func);
    }

private:

    std::vector<Host::Ptr>      hosts;
    std::shared_mutex           mutex;    // From C++14

};

#endif //HOSTS_CONTAINER_H_
