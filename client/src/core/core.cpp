#include "core.h"
#include "sgxlib/net/boost/client.h"
#include "sgxlib/net/ip.h"
#include "../machineid/machineid.h"
#include "../config/config.h"
#include "../common.h"
#include "../bridge/outgoing_forwarder/outgoing_tcp_forwarder_manager.h"
#include "../bridge/outgoing_forwarder/outgoing_udp_forwarder_manager.h"
#include <thread>

#include <qdebug.h>

Core::Core(
    MainEventEmitterInterface &mainWindowEventEmitter,
    ConnectionEventEmitterInterface &incomingConnectionEventEmitter,
    ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
    Store::HostsStoreInterface &hostsStore,
    LogsEventEmitterInterface &logsEmitter
) :
    mainEventEmitter(mainWindowEventEmitter),
    hostsStore(hostsStore),
    outgoingConnectionEventEmitter(outgoingConnectionEventEmitter),
    logsEmitter(logsEmitter),
    bridgeClient(
        mainWindowEventEmitter, 
        incomingConnectionEventEmitter, 
        outgoingConnectionEventEmitter, 
        logsEmitter,
        hostsContainer
    )
{
}

Core::~Core()
{
}

void		Core::Run()
{
    // Set MachineID
    mainEventEmitter.SetHostID(hostID = MachineID::Get());

    // Load Hosts
    loadHosts();

    // Spawn BridgeClient
    spawnBridgeClient();
}


void	Core::AddHost(const QString &hostID)
{
    if (hostsContainer.HostIDExists(hostID) || hostID == MachineID::Get())
        return;

    // Add Host
    addHostInternal(hostID);
    
    // Notify BridgeClient
    bridgeClient.DiscoverHost(hostID);

    // Persit host in config file
    hostsStore.AddHost(hostID);
}

void	Core::addHostInternal(const QString &hostID)
{
    // Create Host
    Host::Ptr host = Host::Ptr(new Host());
    host->ID = hostID;
    std::string loopback_begin = CONFIG_STR("loopback/begin", LOOPBACK_BEGIN).toStdString();
    std::string loopback_end = CONFIG_STR("loopback/end", LOOPBACK_END).toStdString();

    host->LoopBack = QString(SGX::Net::IP::GetNextIP(hostsContainer.Size() == 0 ? loopback_begin : hostsContainer.Back()->LoopBack.toStdString(), loopback_end).c_str());
    host->OutgoingTCPForwardersManager = new Forwarder::Outgoing::TCP::Manager(host->LoopBack.toStdString(), mainEventEmitter, outgoingConnectionEventEmitter, logsEmitter);

    // Save host
    hostsContainer.AddHost(host);

    // Emit AddEvent
    mainEventEmitter.AddHost(host.get());
}

void	Core::loadHosts()
{
    const QStringList &hostsList = hostsStore.LoadHosts();
    for (const auto &hostID : hostsList)
        addHostInternal(hostID);
}

void	Core::DeleteHost(const QString &hostID)
{
    // Find host
    auto targetHost = hostsContainer.FindHostByID(hostID);

    try
    {
        // Delete host, may throw exception if bridge is down
        bridgeClient.DeleteHost(targetHost);
    }
    catch (...) { /* ignored */ }

    // Destroy outgoing tcp forwarders
    if (targetHost->OutgoingTCPForwardersManager)
    {
        targetHost->OutgoingTCPForwardersManager->DestroyOutgoingForwarders();
        // Delete outgoing forwarders manager
        delete targetHost->OutgoingTCPForwardersManager;
    }
    
    // Destroy outgoing udp forwarders
    if (targetHost->OutgoingUDPForwardersManager)
        targetHost->OutgoingUDPForwardersManager->DestroyOutgoingForwarders();

    // remove host from hostcontainer
    hostsContainer.DeleteHost(hostID);

    // Remove host from config file
    hostsStore.DeleteHost(hostID);
    
}

void	Core::spawnBridgeClient()
{
    // CommandServer
    std::unique_ptr<SGX::Net::Client::TCP::Interface> commandServer(
        new SGX::Net::Client::TCP::BoostClient(
            CONFIG_STR("bridge/host", BRIDGE_DEFAULT_HOST).toStdString(),
            CONFIG_STR("bridge/port", BRIDGE_DEFAULT_PORT).toStdString())
    );

    // Spawn thread
    std::thread(
        &BridgeClient::Run,         // Method
        &bridgeClient,              // Target
        std::move(commandServer),   // Arg 1
        hostID.toStdString()        // Arg 2
    ).detach();
}