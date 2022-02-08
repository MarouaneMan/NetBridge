#include "host.h"

Host::Host(SGX::Net::Socket::TCP::Interface::Ptr &Conn) : conn(std::move(Conn)), remoteIP(conn->GetRemoteIP())
{
}

bool            Host::ReadMessage(size_t maxSize)
{
    return conn->ReadMessage(Buffer, maxSize);
}

bool            Host::WriteMessage(const char *data, size_t len_data)
{
    return conn->WriteMessageThSafe(data, len_data);
}

std::string     Host::GetRemoteIP()
{
  return remoteIP;
}

void            Host::AddRelationShip(const std::string &hostID)
{
    // Lock relationships for write
    std::lock_guard<std::mutex> lock(relationships_mutex);
    
    // Save relationship
    relationships.insert(hostID);
}

void            Host::DeleteRelationShip(const std::string &hostID)
{
    // Lock relationships 
    std::lock_guard<std::mutex> lock(relationships_mutex);

    // Delete relationship
    relationships.erase(hostID);
}

size_t           Host::RelationShipsSize()
{
    // Lock relationships 
    std::lock_guard<std::mutex> lock(relationships_mutex);
    return relationships.size();
}
