#include "outgoing_tcp_forwarder_manager.h"
#include "outgoing_tcp_forwarder.h"
#include "sgxlib/net/boost/server.h"
#include "../../config/config.h"
#include <thread>
#include <qdebug.h>
#include <qmap.h>

Forwarder::Outgoing::TCP::Manager::Manager(
    const std::string &loopback,
    MainEventEmitterInterface &mainEventEmitter,
    ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
    LogsEventEmitterInterface &logsEmitter
) : 
    loopback(loopback),
    mainEventEmitter(mainEventEmitter),
    outgoingConnectionEventEmitter(outgoingConnectionEventEmitter),
    logsEmitter(logsEmitter)
{
}

void    Forwarder::Outgoing::TCP::Manager::SpawnOutgoingForwarder(SGX::Net::Socket::TCP::Interface *commandServer, Host::Ptr targetHost)
{
    QStringList tcp_ports = CONFIG_STR("loopback/tcp_ports", "").split(QRegExp("\\s+"), QString::SkipEmptyParts);
    
    // Parse ports mapping
    QStringList tcp_ports_map_s = CONFIG_STR("loopback/tcp_ports_map", "").split(QRegExp("\\s+"), QString::SkipEmptyParts);
    qDebug() << "tcp_ports_map_s = " << tcp_ports_map_s;
    QMap<QString, QString> ports_map;
    for (const auto & map_s : tcp_ports_map_s)
    {
        QStringList list = map_s.split(QRegExp("\\s*->\\s*"), QString::SkipEmptyParts);
        if (list.size() == 2)
            ports_map.insert(list[0], list[1]);
    }

    for (const auto & tcp_port : tcp_ports)
    {
        unsigned short mapped_port = ports_map.contains(tcp_port) ? ports_map[tcp_port].toUShort() : 0;
        unsigned short port = tcp_port.toUShort();
        if (port != 0)
        {
            auto tcpServer = new Outgoing::TCP::TCPForwarder(
                commandServer,
                targetHost,
                SGX::Net::Server::TCP::Interface::Ptr(new SGX::Net::Server::TCP::BoostServer(targetHost->LoopBack.toStdString(), mapped_port ? mapped_port : port)),
                mainEventEmitter,
                outgoingConnectionEventEmitter,
                logsEmitter,
				mapped_port,
                port
            );

            outgoingTcpForwarders[port] = Outgoing::TCP::TCPForwarder::Ptr(tcpServer);

            // Spawn thread
            auto th = std::thread(&Outgoing::TCP::TCPForwarder::RunAcceptor, tcpServer);
            threads.push(std::move(th));
        }
    }

    qDebug() << __FUNCTION__ << "Ports = " << tcp_ports;
}

void    Forwarder::Outgoing::TCP::Manager::StartForward(Command::AttrType::Proto proto, unsigned short remotePort, unsigned short bridgePort, unsigned short forwardID)
{
    if (proto == Command::AttrType::Proto::TCP)
    {
        auto it = outgoingTcpForwarders.find(remotePort);
        if (it != outgoingTcpForwarders.end())
        {
            auto th = std::thread(&Outgoing::TCP::TCPForwarder::StartForward, (Outgoing::TCP::TCPForwarder*)(it->second.get()), forwardID, bridgePort);
            threads.push(std::move(th));
        }
        else
            qDebug() << __FUNCTION__ << "failed to find localTcpServer " << remotePort;
    }
    else if (proto == Command::AttrType::Proto::UDP)
    {
        qDebug() << __FUNCTION__ << "UDP not implemented yet";
    }
    else
    {
        qDebug() << __FUNCTION__ << "unknown protocol";
    }
}

void    Forwarder::Outgoing::TCP::Manager::KillForwardSock(Command::AttrType::Proto proto, unsigned short remotePort, unsigned short forwardID)
{
    if (proto == Command::AttrType::Proto::TCP)
    {
        auto it = outgoingTcpForwarders.find(remotePort);
        if (it != outgoingTcpForwarders.end())
            it->second->KillForward(forwardID);
    }
    else
    {
        qDebug() << __FUNCTION__ << "unknown protocol";
    }
}


void    Forwarder::Outgoing::TCP::Manager::DestroyOutgoingForwarders()
{
    auto it = outgoingTcpForwarders.begin();
    while (it != outgoingTcpForwarders.end())
    {
        qDebug() << __FUNCTION__ << "it->second->Destroy()";
        it->second->Destroy();
        it++;
    }
    while (!threads.empty())
    {
        qDebug() << __FUNCTION__ << "threads.top().join()";
        threads.top().join();
        threads.pop();
    }
    outgoingTcpForwarders.clear();
}