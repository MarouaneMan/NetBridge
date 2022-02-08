#include "forwarder_udp.h"
#include "proto/command.h"
#include "sgxlib/net/ip.h"
#include <algorithm>
#include <ctime>
#include <thread>

Forwarder::UDP::Forwarder::Forwarder(
    SGX::Net::Server::TCP::Interface::Ptr server,
    Host::Ptr host,
    Host::Ptr targetHost,
    SGX::Logger::Interface *logger
) :
    server(std::move(server)),
    host(host),
    targetHost(targetHost),
    logger(logger)
{
}


unsigned short  Forwarder::UDP::Forwarder::Init()
{
    try
    {
        server->Init();
        logger->Info("New UDP Forward server allocated : " + server->GetBoundAddress());
        return server->GetPort();
    }
    catch (std::exception &e)
    {
        logger->Error(std::string("Failed to start UDP Forward server : ") + e.what());
    }
    return 0;

}

void            Forwarder::UDP::Forwarder::Run()
{
    // Log info
    logger->Info("UDP Forwarder spawned for : " + host->HostID);

    // Spawn thread to check if hostPeer & targetPeer are connnected after x seconds
    auto timerTh = std::thread(&Forwarder::forwardTimeoutChecker, this);

    try
    {
        // Accept host peer
        hostPeer = server->Accept();
        logger->Info("UDP Forwarder accepted host peer : " + host->HostID);
        // Read its hostID
        char firstPeerHostID[12] = { 0 };
        hostPeer->ReadAll(&firstPeerHostID[0], sizeof(firstPeerHostID) - 1);

        // Accept target host peer
        targetPeer = server->Accept();
        logger->Info("UDP Forwarder accepted targetHost peer : " + targetHost->HostID);
        // Read its hostID
        char secondPeerHostID[12] = { 0 };
        targetPeer->ReadAll(&secondPeerHostID[0], sizeof(secondPeerHostID) - 1);

        // Swap
        if (std::strcmp(host->HostID.c_str(), &firstPeerHostID[0]) != 0)
            hostPeer.swap(targetPeer);

        // hostPeer connected
        timeout = false;

        // Forward UDP packets
        forward();

        // Destroy server
        logger->Info("UDP Forwarder terminated : " + host->HostID + " => " + targetHost->HostID);
        server->Destroy();
    }
    catch (std::exception &e)
    {
        if (!timeout)
            logger->Error(std::string("UDP Forwarder unexpectedly stopped : ") + e.what() + " : " + host->HostID);
    }
    // Suicide
    timerTh.join();
    delete this;
}

void        Forwarder::UDP::Forwarder::forward()
{
    Command::Request::UDPPacketDesc     UDPDesc;
    Command::Response::UDPPacketDesc    UDPDescResp; // Keep structs (UDPDesc,UDPDescResp) even if they contain the same attributes
    std::vector<char>                   buffer;

    buffer.resize(host->ReadSomeSize);
    try
    {
        while (true)
        {
            // Read UDP packet description
            if (!hostPeer->ReadAll((char*)&UDPDesc, sizeof(UDPDesc)))
                throw std::runtime_error("UDP Forwarder : host peer disconnected" + host->HostID);

            // Check packet size
            auto packetSize = SGX::Net::IP::ntohl((uint32_t)UDPDesc.PacketSize);
            if (packetSize > 1024 * 1024 * 6)
                throw std::runtime_error("UDP Forwarder : hostPeer sent unexpected packet size : " + std::to_string(packetSize) + " => FROM : " + host->HostID);

            // Read data
            if (packetSize > buffer.size()) buffer.resize(packetSize);
            if (!hostPeer->ReadAll(&buffer[0], packetSize))
                throw std::runtime_error("UDP Forwarder : host peer disconnected : " + host->HostID);

            // Send UDP packet description
            UDPDescResp.PacketSize = UDPDesc.PacketSize;
            UDPDescResp.Port = UDPDesc.Port;
            if (!targetPeer->WriteAll((const char *)&UDPDescResp, sizeof(UDPDescResp)))
                throw std::runtime_error("UDP Forwarder : targetHost peer disconnected : " + targetHost->HostID);

            // Send data
            if (!targetPeer->WriteAll(&buffer[0], packetSize))
                throw std::runtime_error("UDP Forwarder : targetHost peer disconnected : " + targetHost->HostID);
        }
    }
    catch (std::exception &e)
    {
        logger->Warning(e.what());
        hostPeer->ForceClose();
        targetPeer->ForceClose();
    }
}

void        Forwarder::UDP::Forwarder::forwardTimeoutChecker()
{
    auto start = std::time(0);
    while (std::time(0) - start < 30 && timeout == true) std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check timeout
    if (timeout == true)
    {
        logger->Error("UDP Forwarder 30 sec timeout : " + host->HostID);
        server->Destroy();
    }
}
