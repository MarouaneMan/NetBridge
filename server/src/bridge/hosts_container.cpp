#include "hosts_container.h"

void     HostsContainer::AddHost(Host::Ptr &host)
{
    std::lock_guard<std::mutex> lock(mutex);
    hosts[host->HostID] = host;
}

void     HostsContainer::DeleteHost(Host::Ptr &host)
{
    std::lock_guard<std::mutex> lock(mutex);
    hosts.erase(host->HostID);
}

Host::Ptr HostsContainer::FindHostByID(const char *id)
{
    std::lock_guard<std::mutex> lock(mutex);

    // Find host
    auto it = hosts.find(id);
    if (it != hosts.end())
        return it->second;
    return nullptr;
}

size_t     HostsContainer::Size()
{
  std::lock_guard<std::mutex> lock(mutex);
  return hosts.size();
}
