#include "outgoing_udp_forwarder_manager.h"
#include "outgoing_udp_forwarder.h"
#include "sgxlib/net/boost/client.h"
#include "sgxlib/net/boost/server.h"
#include "../../config/config.h"
#include "../../common.h"
#include <thread>
#include <qdebug.h>
#include <qmap.h>

Forwarder::Outgoing::UDP::Manager::Manager(
    MainEventEmitterInterface &mainEventEmitter,
    ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
    LogsEventEmitterInterface &logsEmitter
) : 
    mainEventEmitter(mainEventEmitter),
    outgoingConnectionEventEmitter(outgoingConnectionEventEmitter),
    logsEmitter(logsEmitter)
{
}

void    Forwarder::Outgoing::UDP::Manager::Run(std::string hostID, std::string loopback, unsigned short udpBridgePort)
{
    QStringList udp_ports = CONFIG_STR("loopback/udp_ports", "").split(QRegExp("\\s+"), QString::SkipEmptyParts);
    
    // Parse ports mapping
    QStringList udp_ports_map_s = CONFIG_STR("loopback/udp_ports_map", "").split(QRegExp("\\s+"), QString::SkipEmptyParts);
    qDebug() << "udp_ports_map_s = " << udp_ports_map_s;
    QMap<QString, QString> ports_map;
    for (const auto & map_s : udp_ports_map_s)
    {
        QStringList list = map_s.split(QRegExp("\\s*->\\s*"), QString::SkipEmptyParts);
        if (list.size() == 2)
            ports_map.insert(list[0], list[1]);
    }
    
    try
    {
        // Init UDPBridgeClient
        auto bridgeHost = CONFIG_STR("bridge/host", BRIDGE_DEFAULT_HOST).toStdString();
        SGX::Net::Client::TCP::BoostClient udpBrigdeClient(bridgeHost, std::to_string(udpBridgePort));
        auto udpBridgeSock = udpBrigdeClient.Connect();

        // Send HostID
        udpBridgeSock->WriteAll(hostID.c_str(), hostID.size());

        // Foreach UDP Port spawn new UDPForwarder thread
        for (const auto & udp_port : udp_ports)
        {
            unsigned short mapped_port = ports_map.contains(udp_port) ? ports_map[udp_port].toUShort() : 0;
            unsigned short port = udp_port.toUShort();
            if (port != 0)
            {
                auto udpForwarder = new Outgoing::UDP::UDPForwarder(
                    udpBridgeSock.get(),
                    SGX::Net::Server::UDP::Interface::Ptr(new SGX::Net::Server::UDP::BoostServer(loopback, mapped_port ? mapped_port : port)),
                    mainEventEmitter,
                    outgoingConnectionEventEmitter,
                    logsEmitter,
                    mapped_port,
                    port
                );

                outgoingUDPForwarders.push_back(Outgoing::UDP::UDPForwarder::Ptr(udpForwarder));
                
                // Spawn threads
                auto th = std::thread(&Outgoing::UDP::UDPForwarder::Run, udpForwarder);
                threads.push(std::move(th));
            }
        }

        // Clear all
        while (!threads.empty())
        {
            threads.top().join();
            qDebug() << __FUNCTION__ << "threads.top().joined";
            threads.pop();
        }
        outgoingUDPForwarders.clear();
    }
    catch (std::exception &e)
    {
        logsEmitter.Error(QString("Failed to initialize UDPBridgeClient, UDPBridgePort = %1, err = %2").arg(udpBridgePort).arg(e.what()));
    }
    
    qDebug() << __FUNCTION__ << " terminated, ports = " << udp_ports;

    // Delete outgoing forwarders manager
    delete this;
}

void    Forwarder::Outgoing::UDP::Manager::DestroyOutgoingForwarders()
{
    for (int i = 0; i < outgoingUDPForwarders.size(); i++)
    {
        outgoingUDPForwarders[i]->Close();
        qDebug() << __FUNCTION__ << "outgoingUDPForwarders[i]->Close()";
    }
}