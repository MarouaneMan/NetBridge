#include "client.h"
#include "socket.hpp"
#include "../exception.hpp"


// ---- TCP ----

using boost::asio::ip::tcp;

SGX::Net::Client::TCP::BoostClient::BoostClient(const std::string &host, const std::string &port, unsigned int linger_seconds) 
    : BoostBaseClient(host, port), linger_seconds(linger_seconds) {}

SGX::Net::Socket::TCP::Interface::Ptr	SGX::Net::Client::TCP::BoostClient::Connect()
{
    try
    {
        tcp::resolver                   resolver(io_service);
        tcp::resolver::query            query(host, port);
        tcp::resolver::iterator         endpoint_iterator = resolver.resolve(query);

        Socket::TCP::BoostSocket *sock = new Socket::TCP::BoostSocket(tcp::socket(io_service));

        // Connect
        boost::asio::connect(sock->GetSock(), endpoint_iterator);

        // Linger
        sock->Linger(linger_seconds);

        // KeepAlive By default 
        sock->KeepAlive(true);

        // Tcp Nodelay by default
        sock->TcpNoDelay(true);
	
        return SGX::Net::Socket::TCP::Interface::Ptr(sock);
    }
    catch (std::exception &e)
    {
        throw Net::Except::ConnectException(e.what());
    }
}

// ---- UDP ----

using boost::asio::ip::udp;

SGX::Net::Client::UDP::BoostClient::BoostClient(){}

SGX::Net::Socket::UDP::Interface	    *SGX::Net::Client::UDP::BoostClient::Open(const std::string &host, const std::string &port)
{
    try
    {
        udp::resolver                   resolver(io_service);
        udp::resolver::query            query(udp::v4(), host, port);
        udp::endpoint receiver_endpoint = *resolver.resolve(query);

        Socket::UDP::BoostSocket *sock = new Socket::UDP::BoostSocket(udp::socket(io_service), receiver_endpoint);

        // Open
        sock->GetSock().open(udp::v4());
        
        return sock;
    }
    catch (std::exception &e)
    {
        throw Net::Except::OpenException(e.what());
    }
}
