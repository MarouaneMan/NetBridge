#include "forwarder_tcp.h"
#include "sgxlib/net/exception.hpp"
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>

Forwarder::TCP::Forwarder::Forwarder(
    SGX::Net::Server::TCP::Interface::Ptr server,
    SGX::Logger::Interface *logger
) :
    server(std::move(server)),
    logger(logger),
    timeout(true)
{
}

unsigned short  Forwarder::TCP::Forwarder::Init()
{
    try
    {
        server->Init();
        logger->Info("New Tcp Forward server allocated : " + server->GetBoundAddress());
        return server->GetPort();
    }
    catch (std::exception &e)
    {
        logger->Error(std::string("Failed to start Tcp Forward server : ") + e.what());
    }
    return 0;
}

void            Forwarder::TCP::Forwarder::Run(const std::string &hostID, const std::string &targetHostID, unsigned short remotePort, unsigned long readSomeSize)
{
    // Log info
    std::stringstream targetForward;
    targetForward << "(" << hostID << " => " << targetHostID << ") TargetPort = " << remotePort;
    logger->Info("TCP Forwarder spawned for : " + targetForward.str());

    // Spawn thread to check if both peers are connnected after x seconds
    auto timerTh = std::thread(&Forwarder::forwardTimeoutChecker, this, targetForward.str());
    
    try
    {
        // Accept first peer
        firstPeer = server->Accept();
        logger->Info("TCP Forwarder accepted first peer " + targetForward.str());

        // Accept second peer
        secondPeer = server->Accept();
        logger->Info("TCP Forwarder accepted second peer " + targetForward.str());

        // Both peers connected
        timeout = false;

        // Forward packets
        auto th = std::thread(&Forwarder::forward, this, firstPeer.get(), secondPeer.get(), readSomeSize);
        forward(secondPeer.get(), firstPeer.get(), readSomeSize);
        th.join();

        // Destroy server
        logger->Info("TCP Forwarder terminated : " + targetForward.str());
        server->Destroy();
    }
    catch (std::exception &e)
    {
        if (!timeout)
            logger->Error(std::string("TCP Forwarder unexpectedly stopped : ") + e.what() + ":" + targetForward.str());
    }

    // Suicide
    timerTh.join();
    delete this;
}

void        Forwarder::TCP::Forwarder::forward(SGX::Net::Socket::TCP::Interface *src, SGX::Net::Socket::TCP::Interface *dst, unsigned long readSomeSize)
{
    std::vector<char>   buffer;
    
    buffer.resize(readSomeSize);
    while (true)
    {
        // Read from source
        size_t read_size = src->ReadSome(buffer);
        if (read_size == 0)
        {
            // Peer disconnected
            dst->ForceClose();
            logger->Warning("TCP Forwarder : source peer disconnected");
            break;
        }

        // Write to destination
        bool sent = dst->WriteAll(&buffer[0], read_size);
        if (!sent)
        {
            // Peer disconnected
            src->ForceClose();
            logger->Warning("TCP Forwarder : dest peer disconnected");
            break;
        }
    }
}

void       Forwarder::TCP::Forwarder::forwardTimeoutChecker(const std::string &targetForward)
{
    auto start = std::time(0);
    while (std::time(0) - start < 30 && timeout == true) std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check timeout
    if (timeout == true)
    {
        logger->Error("TCP Forwarder 30 sec timeout : " + targetForward);
        server->Destroy();
    }
}
