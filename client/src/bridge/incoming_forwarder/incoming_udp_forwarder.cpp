#include "incoming_udp_forwarder.h"
#include "sgxlib/net/boost/client.h"
#include "sgxlib/net/exception.hpp"
#include "sgxlib/net/ip.h"
#include "../forwarder/forwarder.h"
#include "../forwarder/forwarder_peer.h"
#include "../connection.h"
#include "../../config/config.h"
#include "../../common.h"
#include <qdebug.h>

Forwarder::UDP::IncomingForwarder::IncomingForwarder(
    MainEventEmitterInterface &mainEventEmitter,
    LogsEventEmitterInterface &logsEmitter,
    ConnectionEventEmitterInterface &incomingConnectionEventEmitter

) :
    mainEventEmitter(mainEventEmitter),
    logsEmitter(logsEmitter),
    incomingConnectionEventEmitter(incomingConnectionEventEmitter)
{
}

Forwarder::UDP::IncomingForwarder::~IncomingForwarder()
{
    qDebug() << __FUNCTION__ << " UDP IncomingForwarder destroyed";
}

void    Forwarder::UDP::IncomingForwarder::Run(Command::Response::UDPForwarderEndPoint order, std::string hostID, std::string localIP)
{
    qDebug() << __FUNCTION__ <<  " UDP IncomingForwarder spawned";

    logsEmitter.Info(QString("UDP IncomingForwarder spawned for %1, UDPBridgePort = %2").arg(&order.HostID[0]).arg(order.Port));

    try
    {
        // Init UDPBridgeClient
        auto bridgeHost = CONFIG_STR("bridge/host", BRIDGE_DEFAULT_HOST).toStdString();
        SGX::Net::Client::TCP::BoostClient udpBrigdeClient(bridgeHost, std::to_string(order.Port));
        auto udpBridgeSock = udpBrigdeClient.Connect();

        // Local IP to use, localhost wont work with udp
        if (CONFIG_BOOL("loopback/udp_bypass_loopback", false) == false)
            localIP = "127.0.0.1";
        qDebug() << "UDP Bypass Loopback = " << CONFIG_BOOL("loopback/udp_bypass_loopback", false) << " local_client : " << localIP.c_str();
        
        // Send HostID
        udpBridgeSock->WriteAll(hostID.c_str(), hostID.size());

        Command::Response::UDPPacketDesc    UDPDesc;
        std::vector<char>                   buffer;
        size_t                              buffer_size = 0;
        SGX::Net::Client::UDP::BoostClient  udpClient;

        while (true)
        {
            // Read UDP packet description
            bool read = udpBridgeSock->ReadAll((char*)&UDPDesc, sizeof(UDPDesc));
            if (!read)
                throw std::runtime_error("connection closed");

            // Check packet size
            UDPDesc.PacketSize = SGX::Net::IP::ntohl((uint32_t)UDPDesc.PacketSize);
            if (UDPDesc.PacketSize > 1024 * 1024 * 6)
                throw std::runtime_error("bridge sent unexpected packet size : " + std::to_string(UDPDesc.PacketSize));

            // Resize buffer
            if (UDPDesc.PacketSize > buffer_size)
                buffer.resize(buffer_size = UDPDesc.PacketSize);

            // Read data
            read = udpBridgeSock->ReadAll(&buffer[0], UDPDesc.PacketSize);
            if (!read)
                throw std::runtime_error("connection closed");

            try
            {
                // Find local udp sock or create it
                UDPDesc.Port = SGX::Net::IP::ntohs(UDPDesc.Port);
                auto &sock = mapSock[UDPDesc.Port];
                if (sock.get() == nullptr)
                    sock.reset(udpClient.Open(localIP, std::to_string(UDPDesc.Port)));
                
                // Write all, if it fails destroy the socket
                if (!sock->WriteAll((const char*)&buffer[0], UDPDesc.PacketSize))
                    mapSock.erase(UDPDesc.Port);
            }
            catch (SGX::Net::Except::OpenException &e)
            {
                logsEmitter.Error(QString("Failed to open local udp socket %1, error = %2").arg(UDPDesc.Port).arg(e.what()));
            }
        }
    }
    catch (SGX::Net::Except::ConnectException &e)
    {
        logsEmitter.Error(QString("UDP IncomingForwarder failed to start for %1, error = %2").arg(&order.HostID[0]).arg(e.what()));
    }
    catch (std::exception &e)
    {
        logsEmitter.Error(QString("UDP IncomingForwarder error : %1, HostID : %2").arg(e.what()).arg(&order.HostID[0]));
    }

    qDebug() << __FUNCTION__ << " UDP IncomingForwarder terminated";
    
    // No one will join this thread
    delete this;
}