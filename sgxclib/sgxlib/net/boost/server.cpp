#include "server.h"
#include "socket.hpp"
#include "../exception.hpp"

// --- TCP ---

using boost::asio::ip::tcp;
SGX::Net::Server::TCP::BoostServer::BoostServer(const std::string &address, unsigned int port, unsigned int linger_seconds)
  : BoostBaseServer<SGX::Net::Server::TCP::Interface>(address, port), linger_seconds(linger_seconds)
{
}

SGX::Net::Server::TCP::BoostServer::~BoostServer()
{
    try
    {
    if (acceptor)
        delete acceptor;
    }
    catch (...) {}
}

void	SGX::Net::Server::TCP::BoostServer::Init()
{
    try
    {
        // Bind server to specific interface
        tcp::endpoint endpoint(
            boost::asio::ip::address::from_string(address),
            port
        );

        acceptor = new tcp::acceptor(io_service, endpoint);
        address = acceptor->local_endpoint().address().to_string();
        port = acceptor->local_endpoint().port();
    }
    catch (std::exception &e)
    {
        acceptor = nullptr;
        throw Net::Except::InitException(e.what());
    }
}

SGX::Net::Socket::TCP::Interface::Ptr	SGX::Net::Server::TCP::BoostServer::Accept()
{
    Socket::TCP::BoostSocket *sock = new Socket::TCP::BoostSocket(tcp::socket(io_service));
    try
    {
        acceptor->accept(sock->GetSock());

	    // Linger
	    sock->Linger(linger_seconds);
        
        // KeepAlive by default
        sock->KeepAlive(true);

        // Tcp Nodelay by default
        sock->TcpNoDelay(true);
	
        return SGX::Net::Socket::TCP::Interface::Ptr(sock);
    }
    catch (std::exception &e)
    {
        throw Net::Except::AcceptException(e.what());
    }
}

void            SGX::Net::Server::TCP::BoostServer::Destroy()
{
    try
    {
        if (acceptor)
            acceptor->close();
    }
    catch (...) {}
}

// --- UDP ---

using boost::asio::ip::udp;


SGX::Net::Server::UDP::BoostServer::BoostServer(const std::string &address, unsigned int port)
    : BoostBaseServer<SGX::Net::Server::UDP::Interface>(address, port)
{
}

void					    SGX::Net::Server::UDP::BoostServer::Init()
{
    try
    {
        udp::socket sock = udp::socket(io_service, udp::endpoint(boost::asio::ip::address::from_string(address), port));
        //udp::socket sock = udp::socket(io_service, udp::endpoint(udp::v4(), port));
        port = sock.local_endpoint().port();
        bsocket.reset((Socket::UDP::Interface*)new Socket::UDP::BoostSocket(std::move(sock)));
    }
    catch (std::exception &e)
    {
        throw Net::Except::InitException(e.what());
    }
}

size_t                      SGX::Net::Server::UDP::BoostServer::ReadSome(std::vector<char> &buffer)
{
    return bsocket->ReadSome(buffer);
}

void                        SGX::Net::Server::UDP::BoostServer::Destroy()
{
    bsocket->ForceClose();
}