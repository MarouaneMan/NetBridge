#include "forwarder_peer.h"
#include <qdebug.h>

Forwarder::Peer::TCPPeer::TCPPeer(SGX::Net::Socket::TCP::Interface::Ptr sock) : sock(std::move(sock))
{
}

Forwarder::Peer::TCPPeer::TCPPeer(SGX::Net::Client::TCP::Interface::Ptr client) : client(std::move(client))
{
}

Forwarder::Peer::TCPPeer::~TCPPeer()
{
    if (sock.get())
        sock->ForceClose();
    qDebug() << "TCPPeer destroyed";
}

bool    Forwarder::Peer::TCPPeer::Init()
{
    try
    {
        if (sock.get())
        {
            // Peer already connected
            return true;
        }
        sock = client->Connect();
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
        return false;
    }
    return true;
}

// return 0 on failure
size_t  Forwarder::Peer::TCPPeer::ReadSome(std::vector<char> &buffer)
{
    return sock->ReadSome(buffer);
}

// return false on failure
bool    Forwarder::Peer::TCPPeer::WriteAll(const char *data, size_t len_data)
{
    return sock->WriteAll(data, len_data);
}

void    Forwarder::Peer::TCPPeer::Close()
{
    sock->ForceClose();
}