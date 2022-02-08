#include "outgoing_udp_forwarder.h"
#include "sgxlib/net/exception.hpp"
#include "sgxlib/net/ip.h"
#include "sgxlib/net/boost/client.h"
#include "proto/command.h"
#include "../../config/config.h"
#include "../../common.h"
#include <qstring.h>
#include <limits.h>
#include <thread>
#include <qdebug.h>

Forwarder::Outgoing::UDP::UDPForwarder::UDPForwarder(
    SGX::Net::Socket::TCP::Interface *udpBridgeSock,
    SGX::Net::Server::UDP::Interface::Ptr server,
    MainEventEmitterInterface &mainEventEmitter,
    ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
    LogsEventEmitterInterface &logsEmitter,
	unsigned short mapped_port,
    unsigned short port
) :
    udpBridgeSock(udpBridgeSock),
    server(std::move(server)),
    mainEventEmitter(mainEventEmitter),
    outgoingConnectionEventEmitter(outgoingConnectionEventEmitter),
    logsEmitter(logsEmitter),
	mapped_port(mapped_port),
    port(port)
{
    destroyed = false;
}

Forwarder::Outgoing::UDP::UDPForwarder::~UDPForwarder()
{
    qDebug() << __FUNCTION__ << "Outgoing UDPForwarder destroyed";
}

void    Forwarder::Outgoing::UDP::UDPForwarder::Run()
{
    Command::Request::UDPPacketDesc udpDesc;
    try
    {
        // Init server
        server->Init();

        // Log info
		QString info = QString("Loopback udp server started, listening on <b>%1</b>").arg(server->GetBoundAddress().c_str());
		if (mapped_port) info += QString(" mapped to <b>%1</b>").arg(port);
        logsEmitter.Info(info);

        // Allocate recv buffer
        std::vector<char>   buffer;
        buffer.resize(CONFIG_STR("packet/read_some_size", "4096").toUInt());

        while (true)
        {
            // Read
            size_t read = server->ReadSome(buffer);
            if (read == 0)
                throw std::runtime_error("failed to read udp packet");

            // Write
            udpDesc.PacketSize = SGX::Net::IP::htonl((unsigned long)read);
            udpDesc.Port = SGX::Net::IP::htons(port);

            // Send header
            bool sent = udpBridgeSock->WriteAll((char *)&udpDesc, sizeof(udpDesc));
            if (!sent)
                throw std::runtime_error("failed to send udp header");

            // Sen data
            sent = udpBridgeSock->WriteAll(&buffer[0], read);
            if (!sent)
                throw std::runtime_error("failed to send udp data");
        }
    }
    catch (std::exception &e)
    {
        if (!destroyed)
        {
            // Log error
            logsEmitter.Error(QString("Loopback udp server %1 error : %2")
                .arg(server->GetBoundAddress().c_str())
                .arg(e.what())
            );
        }
        else
        {
            logsEmitter.Info(QString("Loopback udp server %1 closed").arg(server->GetBoundAddress().c_str()));
        }
    }
	qDebug() << __FUNCTION__ << " terminated";
}

void    Forwarder::Outgoing::UDP::UDPForwarder::Close()
{
    destroyed = true;
    udpBridgeSock->ForceClose();
    server->Destroy();
}
