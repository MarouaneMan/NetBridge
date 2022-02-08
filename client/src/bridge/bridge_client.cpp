#include "bridge_client.h"
#include "sgxlib/net/exception.hpp"
#include "sgxlib/net/ip.h"
#include "sgxlib/struct_caster.hpp"
#include "outgoing_forwarder/outgoing_tcp_forwarder_manager.h"
#include "outgoing_forwarder/outgoing_udp_forwarder_manager.h"
#include "incoming_forwarder/incoming_tcp_forwarder.h"
#include "incoming_forwarder/incoming_udp_forwarder.h"
#include "proto/command.h"
#include "../config/config.h"
#include <algorithm>
#include <thread>
#include <chrono>
#include <array>
#include <qdebug.h>

BridgeClient::BridgeClient(
    MainEventEmitterInterface &mainWindowEventEmitter,
    ConnectionEventEmitterInterface &incomingConnectionEventEmitter,
    ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
    LogsEventEmitterInterface &logsEmitter,
    HostsContainer &hostsContainer
) :
    mainEventEmitter(mainWindowEventEmitter),
    incomingConnectionEventEmitter(incomingConnectionEventEmitter),
    outgoingConnectionEventEmitter(outgoingConnectionEventEmitter),
    logsEmitter(logsEmitter),
    hostsContainer(hostsContainer)
{
    // Register response Callbacks
    responseCallbackRegistrator.Register(this, Command::Response::Type::HOST_FOUND, &BridgeClient::hostFoundCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::HOST_NOTFOUND, &BridgeClient::hostNotFoundCallBack);
    responseCallbackRegistrator.Register(this, Command::Response::Type::CURRENT_HOST_DISCOVERED, &BridgeClient::currentHostdiscoveredCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::HOST_DIED, &BridgeClient::friendHostDiedCallback);
	responseCallbackRegistrator.Register(this, Command::Response::Type::HOST_UP, &BridgeClient::friendHostUpCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::RELATIONSHIP_DELETED, &BridgeClient::relationshipDeletedCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::FORWARD_ACCEPTED, &BridgeClient::forwardAcceptedCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::FORWARD_ORDER, &BridgeClient::forwardOrderCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::FORWARD_ERROR, &BridgeClient::forwardErrorCallback);
    responseCallbackRegistrator.Register(this, Command::Response::Type::UDP_FORWARDER_ENDPOINT, &BridgeClient::udpForwardEndpoint);
}

BridgeClient::~BridgeClient()
{
}


void BridgeClient::Run(SGX::Net::Client::TCP::Interface::Ptr commandServer, std::string &hostID)
{
    // hostID
    this->hostID = hostID;

    // Set UI BridgeAddress
    mainEventEmitter.SetBridgeAddress(commandServer->GetServerAddress().c_str());

    // Reconnect delay in seconds
#ifdef _DEBUG 
	std::array<int, 9> reconnectDelay = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
#else
	std::array<int, 9> reconnectDelay = { 600, 120, 60, 30, 20, 10, 5, 2, 1 };
#endif
    int reconnectDelayIndex = (int)reconnectDelay.size();

    while (true)
    {
        try
        {
            // Connect to bridge command server
            commandServerSock = commandServer->Connect();

            // Reset reconnect delay counter
            reconnectDelayIndex = reconnectDelay.size();

            // Set State
            mainEventEmitter.SetBridgeState(State::Connected);

            // Log Connect Success
            logsEmitter.Success(QString("Connected to %1").arg(commandServer->GetServerAddress().c_str()));

            // Register Host
            registerHost();

            // Discover Hosts;
            discoverHosts();

            // Listen for responses
            listenForResponses();
        }
        catch (SGX::Net::Except::ConnectException &e)
        {
            // Log Error
            logsEmitter.Error(QString("Unable to connect to %1 : %2").arg(commandServer->GetServerAddress().c_str()).arg(e.what()));
        }
        catch (SGX::Net::Except::HostDisconnectedException &e)
        {
            // Log Error
            logsEmitter.Error(QString("BridgeServer disconnected : %1").arg(e.what()));
        }
        catch (SGX::StructCaster::Exception::BadSizeException &e)
        {
            // Log Error
            logsEmitter.Error(QString("BridgeServer sent unexpected response size : %1").arg(e.what()));
        }
        catch (SGX::Callback::Exception::UnknownCallbackException &)
        {
            // Log Error
            logsEmitter.Error(QString("BridgeServer sent unknown response"));
        }
        
        // Set Disconnected State
        mainEventEmitter.SetBridgeState(State::Disconnected);

        // Clear OutgoingForwarders
        hostsContainer.ForeachWriteLock([](Host::Ptr h) {
            
            if (h->OutgoingTCPForwardersManager)
            {
                h->OutgoingTCPForwardersManager->DestroyOutgoingForwarders();
                h->OutgoingTCPForwardersManager = nullptr;
            }
            if (h->OutgoingUDPForwardersManager)
            {
                h->OutgoingUDPForwardersManager->DestroyOutgoingForwarders();
                h->OutgoingUDPForwardersManager = nullptr;
            }
        });

        // Reconnection delay
        reconnectDelayIndex = reconnectDelayIndex - 1 < 0 ? 0 : --reconnectDelayIndex;
        auto delayTime = reconnectDelay[reconnectDelayIndex];
        logsEmitter.Info(QString("Retrying reconnection after %1 seconds").arg(delayTime));
        std::this_thread::sleep_for(std::chrono::seconds(delayTime));
    }
}

void	BridgeClient::registerHost()
{
    // Register Request
    Command::Request::Register req;
    req.Type = Command::Request::Type::REGISTER;
    std::strcpy(req.Hostname.data(), SGX::Net::IP::GetHostname().data());
    std::strcpy(req.HostID.data(), hostID.data());
    std::strcpy(req.LocalIP.data(), commandServerSock->GetLocalIP().data());
    req.ReadSomeSize = SGX::Net::IP::htonl(CONFIG_STR("packet/read_some_size", "4096").toUInt());

    // Send Request
    auto sent = commandServerSock->WriteMessageThSafe(reinterpret_cast<const char*>(&req), sizeof(req));
    if (!sent)
        throw SGX::Net::Except::HostDisconnectedException("Failed to send register request");

    logsEmitter.Success("Current Host registered");
}

void	BridgeClient::discoverHosts()
{
    hostsContainer.ForeachReadLock([&](Host::Ptr host) {
        DiscoverHost(host->ID);
    });
}

void	BridgeClient::DiscoverHost(const QString &targetHostID)
{
    if (commandServerSock.get() == nullptr) //ignore request if bridge is down
        return;

    // Discover Request
    Command::Request::DiscoverHost req;
    req.Type = Command::Request::Type::DISCOVER_HOST;
    req.UDPEnabled = CONFIG_STR("loopback/udp_ports", "").trimmed().size() != 0;
    std::strcpy(req.HostID.data(), targetHostID.toStdString().data());

    // Send Request
    auto sent = commandServerSock->WriteMessageThSafe(reinterpret_cast<const char*>(&req), sizeof(req));
    if (!sent)
        throw SGX::Net::Except::HostDisconnectedException("Failed to send discover_host request");
}

void    BridgeClient::DeleteHost(Host::Ptr targetHost)
{
    if (commandServerSock.get() == nullptr) //ignore request if bridge is down
        return;

    // Delete Host Relationship
    Command::Request::DeleteRelationship req;
    req.Type = Command::Request::Type::DELETE_RELATIONSHIP;
    std::strcpy(req.HostID.data(), targetHost->ID.toStdString().data());

    // Send Request
    auto sent = commandServerSock->WriteMessageThSafe(reinterpret_cast<const char*>(&req), sizeof(req));
    if (!sent)
        throw SGX::Net::Except::HostDisconnectedException("Failed to send delete_relationship request");
}


void    BridgeClient::hostFoundCallback()
{
    Command::Response::HostFound *resp = SGX::StructCaster::Vector::Eq<Command::Response::HostFound>(commandBuffer);
    
    auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);
    targetHost->State = Host::State::Up;
    targetHost->HostName = &resp->Hostname[0];
    targetHost->IpAddress = &resp->LocalIP[0];
    
    logsEmitter.Success(QString("HostID : %1 discovered, Hostname = %2, LocalIP = %3")
        .arg(targetHost->ID)
        .arg(targetHost->HostName)
        .arg(targetHost->IpAddress));

    mainEventEmitter.UpdateHost(targetHost.get());

    // Spawn outgoing forwarder
    targetHost->OutgoingTCPForwardersManager->SpawnOutgoingForwarder(commandServerSock.get(), targetHost);
}

void    BridgeClient::hostNotFoundCallBack()
{
    Command::Response::HostNotFound *resp = SGX::StructCaster::Vector::Eq<Command::Response::HostNotFound>(commandBuffer);

    // Find targetHost
    auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);
    
    targetHost->State = Host::State::Down;

    logsEmitter.Warning(QString("Failed to discover HostID : %1").arg(targetHost->ID));

    mainEventEmitter.UpdateHost(targetHost.get());
}

void    BridgeClient::currentHostdiscoveredCallback()
{
    Command::Response::CurrentHostDiscovered *resp = SGX::StructCaster::Vector::Eq<Command::Response::CurrentHostDiscovered>(commandBuffer);

    logsEmitter.Info(QString("Current Host discovered by %1, Hostname = %2, LocalIP = %3")
        .arg(&resp->HostID[0])
        .arg(&resp->Hostname[0])
        .arg(&resp->LocalIP[0])
    );
}

void    BridgeClient::friendHostDiedCallback()
{
    Command::Response::HostDied *resp = SGX::StructCaster::Vector::Eq<Command::Response::HostDied>(commandBuffer);

    // Find targetHost
    auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);
    if (targetHost.get() != nullptr)
    {
        targetHost->State = Host::State::Down;

        logsEmitter.Warning(QString("Host %1 died").arg(targetHost->ID));

        mainEventEmitter.UpdateHost(targetHost.get());

        // Destroy outgoing forwarders
        targetHost->OutgoingTCPForwardersManager->DestroyOutgoingForwarders();
        if (targetHost->OutgoingUDPForwardersManager)
        {
            targetHost->OutgoingUDPForwardersManager->DestroyOutgoingForwarders();
            targetHost->OutgoingUDPForwardersManager = nullptr;
        }
    }
    else
    {
        // TargetHost isnt in our friends list
        logsEmitter.Warning(QString("Host %1 died").arg(&resp->HostID[0]));
    }
}

void       BridgeClient::friendHostUpCallback()
{
	Command::Response::HostDied *resp = SGX::StructCaster::Vector::Eq<Command::Response::HostDied>(commandBuffer);

	// Find targetHost
	auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);
	if (targetHost.get() != nullptr)
	{
		if (targetHost->State == Host::State::Down)
			DiscoverHost(targetHost->ID);
	}
	else
	{
		// TargetHost isnt in our friends list
		logsEmitter.Success(QString("Host %1 up").arg(&resp->HostID[0]));
	}
}

void       BridgeClient::relationshipDeletedCallback()
{
    Command::Response::RelationshipDeleted *resp = SGX::StructCaster::Vector::Eq<Command::Response::RelationshipDeleted>(commandBuffer);
    
    logsEmitter.Warning(QString("Host %1 deleted you").arg(&resp->HostID[0]));
}

void        BridgeClient::forwardAcceptedCallback()
{
    Command::Response::ForwardAccepted *resp = SGX::StructCaster::Vector::Eq<Command::Response::ForwardAccepted>(commandBuffer);

    // Find targetHost
    auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);

    // Endianess
    resp->ForwardID = SGX::Net::IP::ntohs(resp->ForwardID);
    resp->Protocol = (Command::AttrType::Proto)SGX::Net::IP::ntohs(resp->Protocol);
    resp->RemotePort = SGX::Net::IP::ntohs(resp->RemotePort);
    resp->BridgeRandomPort = SGX::Net::IP::ntohs(resp->BridgeRandomPort);

    // Log info
    logsEmitter.Success(QString("Forward request accepted, TargetHostID : %1, RemotePort : %2, BridgePort : %3, Proto : %4")
        .arg(targetHost->ID)
        .arg(resp->RemotePort)
        .arg(resp->BridgeRandomPort)
        .arg(resp->Protocol == Command::AttrType::Proto::TCP ? "TCP" : "UDP")
    );

    // Start forward
    targetHost->OutgoingTCPForwardersManager->StartForward(resp->Protocol, resp->RemotePort, resp->BridgeRandomPort, resp->ForwardID);
}

void        BridgeClient::forwardOrderCallback()
{
    Command::Response::ForwardOrder *resp = SGX::StructCaster::Vector::Eq<Command::Response::ForwardOrder>(commandBuffer);

    SGX::String::NullTerminateStdArray(resp->HostID);
    SGX::String::NullTerminateStdArray(resp->FromHostname);
    SGX::String::NullTerminateStdArray(resp->FromLocalIP);

    // Endianess
    resp->Protocol = (Command::AttrType::Proto)SGX::Net::IP::ntohs(resp->Protocol);
    resp->LocalPort = SGX::Net::IP::ntohs(resp->LocalPort);
    resp->BridgeRandomPort = SGX::Net::IP::ntohs(resp->BridgeRandomPort);
    resp->ReadSomeSize = SGX::Net::IP::ntohl(static_cast<unsigned long>(resp->ReadSomeSize));

    // Spawn Remote Forwarder
    auto incomingForwarder = new Forwarder::TCP::IncomingForwarder(mainEventEmitter, logsEmitter, incomingConnectionEventEmitter);
    std::thread(&Forwarder::TCP::IncomingForwarder::Run, incomingForwarder, *resp, commandServerSock->GetLocalIP()).detach();

    // Log info
    logsEmitter.Info(
        QString("Received new Forward order from HostID = %1, Hostname = %2, TargetLocalPort = %3, BridgePort = %4, Proto = %5")
        .arg(&resp->HostID[0])
        .arg(&resp->FromHostname[0])
        .arg(resp->LocalPort)
        .arg(resp->BridgeRandomPort)
        .arg(resp->Protocol == Command::AttrType::Proto::TCP ? "TCP" : "UDP")
    );
}

void        BridgeClient::forwardErrorCallback()
{
    Command::Response::ForwardError *resp = SGX::StructCaster::Vector::Eq<Command::Response::ForwardError>(commandBuffer);

    // Find targetHost
    auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);

    // Endianess
    resp->ForwardID = SGX::Net::IP::ntohs(resp->ForwardID);
    resp->Protocol = (Command::AttrType::Proto)SGX::Net::IP::ntohs(resp->Protocol);
    resp->RemotePort = SGX::Net::IP::ntohs(resp->RemotePort);

    // Kill
    targetHost->OutgoingTCPForwardersManager->KillForwardSock(resp->Protocol, resp->RemotePort, resp->ForwardID);

    // Log
    logsEmitter.Error(QString("BridgeServer failed to initialize ForwardServer, HostID = %1, Proto = %2, RemotePort = %3")
        .arg(&resp->HostID[0])
        .arg(resp->Protocol)
        .arg(resp->RemotePort)
    );
}

void        BridgeClient::udpForwardEndpoint()
{
    Command::Response::UDPForwarderEndPoint *resp = SGX::StructCaster::Vector::Eq<Command::Response::UDPForwarderEndPoint>(commandBuffer);

    // Find targetHost
    auto targetHost = hostsContainer.FindHostByID(&resp->HostID[0]);

    // Endianess
    resp->Port = SGX::Net::IP::ntohs(resp->Port);

    qDebug() << "Received new udpForwardEndpoint from " << &resp->HostID[0] << " | Port : " << resp->Port;
    if (targetHost.get() != nullptr)
    {
        // Run Outgoing udp forwarder
        targetHost->OutgoingUDPForwardersManager = new Forwarder::Outgoing::UDP::Manager(
            mainEventEmitter,
            outgoingConnectionEventEmitter,
            logsEmitter
        );
        std::thread(&Forwarder::Outgoing::UDP::Manager::Run, targetHost->OutgoingUDPForwardersManager, this->hostID, targetHost->LoopBack.toStdString(), resp->Port).detach();
    }
    else
    {
        // Spawn Remote Forwarder
        auto incomingForwarder = new Forwarder::UDP::IncomingForwarder(mainEventEmitter, logsEmitter, incomingConnectionEventEmitter);
        std::thread(&Forwarder::UDP::IncomingForwarder::Run, incomingForwarder, *resp, this->hostID, commandServerSock->GetLocalIP()).detach();
    }
}

void       BridgeClient::listenForResponses()
{
    // Read Response
    while (commandServerSock->ReadMessage(commandBuffer, 512))
    {
        // Parse response
        Command::Response::BaseResponse *baseResp = SGX::StructCaster::Vector::Gte<Command::Response::BaseResponse>(commandBuffer);
        
        // Call response callback
        responseCallbackRegistrator.Call(baseResp->Type);
    }
}