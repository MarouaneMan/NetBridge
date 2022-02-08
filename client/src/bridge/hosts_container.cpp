#include "hosts_container.h"
#include <algorithm>

HostsContainer::HostsContainer()
{
}

HostsContainer::~HostsContainer()
{
}

void        HostsContainer::AddHost(Host::Ptr host)
{
    // Lock hosts for write
    std::lock_guard<std::shared_mutex> lock(mutex);

    hosts.push_back(host);
}

void        HostsContainer::DeleteHost(const QString &ID)
{
    // Lock hosts for write
    std::lock_guard<std::shared_mutex> lock(mutex);

    // Erase
    hosts.erase(
        std::remove_if(hosts.begin(), hosts.end(), 
            [&](Host::Ptr host) {  return  host->ID == ID; }
        ), 
        hosts.end()
    );
}

Host::Ptr   HostsContainer::FindHostByID(const QString &hostID)
{
    // Lock hosts for read
    std::shared_lock<std::shared_mutex> lock(mutex);
    
    // Find host by id
    auto host = std::find_if(hosts.begin(), hosts.end(), [&](Host::Ptr host) {
        return host->ID == hostID;
    });
    if (host == hosts.end())
        return nullptr;
    return *host;
}

bool    HostsContainer::HostIDExists(const QString &hostID)
{
    // Lock hosts for read
    std::shared_lock<std::shared_mutex> lock(mutex);
    
    // Find host by id
    auto host = std::find_if(hosts.begin(), hosts.end(), [&](Host::Ptr host) {
        return host->ID == hostID;
    });
    return host != hosts.end();
}

size_t   HostsContainer::Size()
{
    // Lock hosts for read
    std::shared_lock<std::shared_mutex> lock(mutex);

    return hosts.size();
}

Host::Ptr HostsContainer::Back()
{
    // Lock hosts for read
    std::shared_lock<std::shared_mutex> lock(mutex);
    
    return hosts.back();
}