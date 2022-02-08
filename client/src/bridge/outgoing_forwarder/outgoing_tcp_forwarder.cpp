#include "outgoing_tcp_forwarder.h"
#include "sgxlib/net/exception.hpp"
#include "sgxlib/net/ip.h"
#include "sgxlib/net/boost/client.h"
#include "sgxlib/rand.hpp"
#include "proto/command.h"
#include "../../config/config.h"
#include "../../common.h"
#include "../forwarder/forwarder_peer.h"
#include <qstring.h>
#include <limits.h>
#include <thread>
#include <qdebug.h>

Forwarder::Outgoing::TCP::TCPForwarder::TCPForwarder(
    SGX::Net::Socket::TCP::Interface *commandServer,
    Host::Ptr targetHost,
    SGX::Net::Server::TCP::Interface::Ptr server,
    MainEventEmitterInterface &mainEventEmitter,
    ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
    LogsEventEmitterInterface &logsEmitter,
	unsigned short mapped_port,
    unsigned short port
) :
    commandServer(commandServer),
    targetHost(targetHost),
    server(std::move(server)),
    mainEventEmitter(mainEventEmitter),
    outgoingConnectionEventEmitter(outgoingConnectionEventEmitter),
    logsEmitter(logsEmitter),
	mapped_port(mapped_port),
    port(port)
{
    destroyed = false;
}

Forwarder::Outgoing::TCP::TCPForwarder::~TCPForwarder()
{
    qDebug() << __FUNCTION__ << "Outgoing TCPForwarder destroyed";
}

void    Forwarder::Outgoing::TCP::TCPForwarder::RunAcceptor()
{
    try
    {
        // Init server
        server->Init();

        // Log info
		QString info = QString("Loopback tcp server started, listening on <b>%1</b>").arg(server->GetBoundAddress().c_str());
		if (mapped_port) info += QString(" mapped to <b>%1</b>").arg(port);
        logsEmitter.Info(info);

        // Rand
        SGX::Rand::Bounded<unsigned short> rand(0, USHRT_MAX);

        // Accept connections
        while (SGX::Net::Socket::TCP::Interface::Ptr sock = server->Accept())
        {
            
            // Forward request
            Command::Request::Forward req;
            req.Type            = Command::Request::Type::FORWARD;
            std::strcpy(req.HostID.data(), targetHost->ID.toStdString().data());
            unsigned short forwardID = rand.Next();
            req.ForwardID       = SGX::Net::IP::htons(forwardID);
            req.Protocol        = (Command::AttrType::Proto)SGX::Net::IP::htons(Command::AttrType::Proto::TCP);
            req.RemotePort      = SGX::Net::IP::htons(port);

            // Send forward request
            auto sent = commandServer->WriteMessageThSafe(reinterpret_cast<const char*>(&req), sizeof(req));
            if (!sent)
            {
                // Log Error
                logsEmitter.Error(QString("Failed to send forward request from %1 to HostID %2")
                    .arg(server->GetBoundAddress().c_str())
                    .arg(targetHost->ID)
                );
                continue;
            }

            // Make SockInfo
            std::lock_guard<std::mutex> lock(mutex);
            auto sockInfo = new Connection::SockInfo;
            sockInfo->Direction = Connection::OUTGOING;
            sockInfo->ConnInfo.HostID = targetHost->ID;
            sockInfo->ConnInfo.Hostname = targetHost->HostName;
            sockInfo->ConnInfo.IPAddress= targetHost->IpAddress;
            sockInfo->ConnInfo.Proto = Connection::Proto::TCP;
			sockInfo->ConnInfo.TargetPort = port;
            sockInfo->ReadSomeSize = CONFIG_STR("packet/read_some_size", "4096").toUInt();

			// Make ForwardInfo
			Info *forwardInfo = new Info;
			forwardInfo->LockSock = std::move(sock);
			forwardInfo->SockInfo = Connection::SockInfo::Ptr(sockInfo);
			sockMap[forwardID] = std::unique_ptr<Info>(forwardInfo);

            // New connection
            qDebug() << __FUNCTION__ <<  " Received connection from loopback " << server->GetBoundAddress().c_str() << " stacked";
        }
    }
    catch (std::exception &e)
    {
        if (!destroyed)
        {
            // Update host color
            targetHost->State = Host::Error;
            mainEventEmitter.UpdateHost(targetHost.get());

            // Log error
            logsEmitter.Error(QString("Loopback tcp server %1 error, HostID : %2 : %3")
                .arg(server->GetBoundAddress().c_str())
                .arg(targetHost->ID)
                .arg(e.what())
            );
        }
        else
        {
            logsEmitter.Info(QString("Loopback tcp server %1 closed").arg(server->GetBoundAddress().c_str()));
        }
    }
	qDebug() << __FUNCTION__ << " terminated";
}

void    Forwarder::Outgoing::TCP::TCPForwarder::Destroy()
{
    destroyed = true;

	{
		// Close all forwarders
		std::lock_guard<std::mutex> lock(mutex);
		auto it = sockMap.begin();
		while (it != sockMap.end())
		{
			it->second->Forwarder->Close();
			it++;
		}
	}

	// Close server
    server->Destroy();
}

void    Forwarder::Outgoing::TCP::TCPForwarder::StartForward(unsigned short forwardID, unsigned short bridgeRandomPort)
{
    qDebug() << __FUNCTION__ << "OutgoingForwarder spawned : bridgeRandomPort = " << bridgeRandomPort;

    // Lock sockMap
    mutex.lock();
    auto it = sockMap.find(forwardID);
    if (it != sockMap.end())
    {
		
		it->second->SockInfo->ConnInfo.BridgePort = bridgeRandomPort;
        Connection::ConnInfo connInfo = it->second->SockInfo->ConnInfo;

        // Notify Main
        mainEventEmitter.UpdateOutgoingConnectoinsCounter(1);
        outgoingConnectionEventEmitter.AddConnection(connInfo);

        // Local Peer
        auto localPeer = new Forwarder::Peer::TCPPeer(std::move(it->second->LockSock));
        
        // Remote Peer
        auto bridgeHost = CONFIG_STR("bridge/host", BRIDGE_DEFAULT_HOST).toStdString();
        auto bridgeForwardClient = new SGX::Net::Client::TCP::BoostClient(bridgeHost, std::to_string(bridgeRandomPort));
        auto remotePeer = new Forwarder::Peer::TCPPeer(SGX::Net::Client::TCP::Interface::Ptr(bridgeForwardClient));

        // Forwarder
        auto forwarder = Forwarder::Forward(
            std::move(it->second->SockInfo),
            Forwarder::Peer::Interface::Ptr(localPeer), 
            Forwarder::Peer::Interface::Ptr(remotePeer),
            logsEmitter
        );
		it->second->Forwarder = &forwarder;

		// unlock sockMap
        mutex.unlock();

        // Start Forwarder
        forwarder.Run();

		// Delete Forwarder
		{
			std::lock_guard<std::mutex>	lock(mutex);
			sockMap.erase(it);
		}

        // Notify Main
        mainEventEmitter.UpdateOutgoingConnectoinsCounter(-1);
        outgoingConnectionEventEmitter.DeleteConnection(connInfo);
    }
    else
    {
        qDebug() << __FUNCTION__ << " failed to find forwardID : " << forwardID;
        
        // unlock sockMap
        mutex.unlock();
    }
	
	qDebug() << __FUNCTION__ << " terminated";
}

void    Forwarder::Outgoing::TCP::TCPForwarder::KillForward(unsigned short forwardID)
{
    std::lock_guard<std::mutex> lock(mutex);
	auto it = sockMap.find(forwardID);
	if (it != sockMap.end())
		it->second->Forwarder->Close();
}