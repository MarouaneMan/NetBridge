#include "incoming_tcp_forwarder.h"
#include "sgxlib/net/boost/client.h"
#include "../forwarder/forwarder.h"
#include "../forwarder/forwarder_peer.h"
#include "../connection.h"
#include "../../config/config.h"
#include "../../common.h"
#include <qdebug.h>

Forwarder::TCP::IncomingForwarder::IncomingForwarder(
    MainEventEmitterInterface &mainEventEmitter,
    LogsEventEmitterInterface &logsEmitter,
    ConnectionEventEmitterInterface &incomingConnectionEventEmitter

) :
    mainEventEmitter(mainEventEmitter),
    logsEmitter(logsEmitter),
    incomingConnectionEventEmitter(incomingConnectionEventEmitter)
{
}

Forwarder::TCP::IncomingForwarder::~IncomingForwarder()
{
    qDebug() << __FUNCTION__ << " TCP IncomingForwarder destroyed";
}

void    Forwarder::TCP::IncomingForwarder::Run(Command::Response::ForwardOrder order, std::string localIP)
{
    qDebug() << __FUNCTION__ <<  " TCP IncomingForwarder spawned";

    // Make connectionInfo
    Connection::ConnInfo connInfo;
    connInfo.HostID = &order.HostID[0];
    connInfo.Hostname = &order.FromHostname[0];
    connInfo.IPAddress = &order.FromLocalIP[0];
    connInfo.Proto = Connection::Proto::TCP;
    connInfo.BridgePort = order.BridgeRandomPort;
    connInfo.TargetPort = order.LocalPort;

    // Make sockInfo sock
    auto sockInfo = Connection::SockInfo::Ptr(new Connection::SockInfo);
    sockInfo->Direction = Connection::INCOMING;
    sockInfo->ConnInfo = connInfo;
    sockInfo->ReadSomeSize = (unsigned long)order.ReadSomeSize;

    // Notify Main
    mainEventEmitter.UpdateIncomingConnectionsCounter(1);
    incomingConnectionEventEmitter.AddConnection(connInfo);

    // Remote Client
    auto bridgeHost = CONFIG_STR("bridge/host", BRIDGE_DEFAULT_HOST).toStdString();
    auto remoteClient = new SGX::Net::Client::TCP::BoostClient(bridgeHost, std::to_string(order.BridgeRandomPort));
    auto remotePeer = new Forwarder::Peer::TCPPeer(SGX::Net::Client::TCP::Interface::Ptr(remoteClient));

    // Local Client
	if (CONFIG_BOOL("loopback/tcp_bypass_loopback", false) == false)
		localIP = "localhost";
	qDebug() << "TCP Bypass Loopback = " << CONFIG_BOOL("loopback/tcp_bypass_loopback", false) << " local_client : " << localIP.c_str();

    auto localClient = new SGX::Net::Client::TCP::BoostClient(localIP, std::to_string(order.LocalPort));
    auto localPeer = new Forwarder::Peer::TCPPeer(SGX::Net::Client::TCP::Interface::Ptr(localClient));

    // Start Forwarder
    auto forwarder = Forwarder::Forward(
        std::move(sockInfo),
        Forwarder::Peer::Interface::Ptr(remotePeer),
        Forwarder::Peer::Interface::Ptr(localPeer),
        logsEmitter
    );
    forwarder.Run();

    // Notify Main
    mainEventEmitter.UpdateIncomingConnectionsCounter(-1);
    incomingConnectionEventEmitter.DeleteConnection(connInfo);

    // No one will join this thread
    delete this;
}