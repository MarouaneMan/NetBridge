#include "forward.h"
#include "proto/command.h"
#include "sgxlib/struct_caster.hpp"
#include "sgxlib/string.hpp"
#include "sgxlib/net/ip.h"
#include "sgxlib/net/boost/server.h"
#include "sgxlib/net/exception.hpp"
#include "../forwarder/forwarder_tcp.h"
#include <thread>

Command::Runner::Forward::Forward(SGX::Logger::Interface *logger, HostsContainer *hostsContainer)
  : Abstract(logger, hostsContainer)
{
}

void     Command::Runner::Forward::Run(Host::Ptr &host)
{
    // Parse Request
    Command::Request::Forward *req = SGX::StructCaster::Vector::Eq<Command::Request::Forward>(host->Buffer);

    // Endianess
    req->ForwardID = SGX::Net::IP::ntohs(req->ForwardID);
    req->Protocol = (Command::AttrType::Proto)SGX::Net::IP::ntohs(req->Protocol);
    req->RemotePort = SGX::Net::IP::ntohs(req->RemotePort);

    // Find target host
    SGX::String::NullTerminateStdArray(req->HostID);
    auto targetHost = hostsContainer->FindHostByID(&req->HostID[0]);

    // Host found
    if (targetHost != nullptr)
    {
        // DEBUG
        std::stringstream ss;
        ss << "HostID : " << host->HostID
            << " forwardID : " << req->ForwardID
            << " requested forwrard to " << &req->HostID[0]
            << " RemotePort : " << req->RemotePort
            << " ReadSome Size : " << host->ReadSomeSize;
        logger->Debug(ss.str());

        if (req->Protocol == Command::AttrType::TCP)
        {
            forwardTCP(host, targetHost, req);
        }
        else
        {
            logger->Error("HostID : " + host->HostID + " requested forward with unknown protocol : " + std::to_string(req->Protocol));
        }
    }
    // Host not found
    else
    {
        logger->Warning("Host : " + host->HostID + " forward to unknown host : " + &req->HostID[0]);
    }
}

void        Command::Runner::Forward::forwardTCP(Host::Ptr &host, Host::Ptr &targetHost, Request::Forward *req)
{
    logger->Info("HostID : " + host->HostID + " requested forward to " 
        + targetHost->HostID + ", TCP-RemotePort = " + std::to_string(req->RemotePort) 
        + ", ReadSomeSize = " + std::to_string(host->ReadSomeSize));

    // 1 - Allocate new TCPForwarder
    Forwarder::TCP::Forwarder *tcp_forwarder = new Forwarder::TCP::Forwarder(
        std::unique_ptr<SGX::Net::Server::TCP::Interface>(new SGX::Net::Server::TCP::BoostServer("0.0.0.0", 0)),
        logger
    );

    // 2 - Init & Get Bound port
    unsigned short port = tcp_forwarder->Init();
    if (port > 0)
    {
        try
        {
            // Send forwardAccepted to host
            sendForwardAcceptedToHost(host, targetHost, req, port);

            // Send Forward to targetHost
            if (sendForwardOrderToTargetHost(host, targetHost, req, port))
            {
                // Spawn tcp_forwarder thread
                std::thread(
                    &Forwarder::TCP::Forwarder::Run,
                    tcp_forwarder,
                    host->HostID, targetHost->HostID, req->RemotePort, host->ReadSomeSize
                ).detach();
            }
            else
            {
                logger->Error("Failed to send forward order to target host : " + targetHost->HostID);
                delete tcp_forwarder;
            }
        }
        catch (SGX::Net::Except::HostDisconnectedException &e)
        {
            delete tcp_forwarder;
            throw SGX::Net::Except::HostDisconnectedException(e);
        }
    }
    else
    {
        // Failed to initialize tcp server
        sendForwardErrorToHost(host, targetHost, req);
    }
}

void        Command::Runner::Forward::sendForwardAcceptedToHost(Host::Ptr &host, Host::Ptr &targetHost, Request::Forward *req, unsigned short bridgeRandomPort)
{
    // ForwardAccepted response
    Command::Response::ForwardAccepted resp;
    resp.Type = Command::Response::Type::FORWARD_ACCEPTED;
    std::strcpy(resp.HostID.data(), targetHost->HostID.c_str());
    resp.ForwardID = SGX::Net::IP::htons(req->ForwardID);
    resp.Protocol = (Command::AttrType::Proto)SGX::Net::IP::htons(req->Protocol);
    resp.RemotePort = SGX::Net::IP::htons(req->RemotePort);
    resp.BridgeRandomPort = SGX::Net::IP::htons(bridgeRandomPort);

    // Send response
    auto sent = host->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
    if (!sent)
        throw SGX::Net::Except::HostDisconnectedException("Failed to send ForwardAccepted response");
}

bool        Command::Runner::Forward::sendForwardOrderToTargetHost(Host::Ptr &host, Host::Ptr &targetHost, Request::Forward *req, unsigned short bridgeRandomPort)
{
    // ForwardOrder response
    Command::Response::ForwardOrder resp;
    resp.Type = Command::Response::Type::FORWARD_ORDER;
    std::strcpy(resp.HostID.data(), host->HostID.c_str());
    std::strcpy(resp.FromHostname.data(), host->Hostname.c_str());
    std::strcpy(resp.FromLocalIP.data(), host->LocalIP.c_str());
    resp.Protocol = (Command::AttrType::Proto)SGX::Net::IP::htons(req->Protocol);
    resp.ReadSomeSize = SGX::Net::IP::htonl(static_cast<uint32_t>(host->ReadSomeSize));
    resp.BridgeRandomPort = SGX::Net::IP::htons(bridgeRandomPort);
    resp.LocalPort = SGX::Net::IP::htons(req->RemotePort);

    // Send order
    return targetHost->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
}

void        Command::Runner::Forward::sendForwardErrorToHost(Host::Ptr &host, Host::Ptr &targetHost, Request::Forward *req)
{
    // ForwardError response
    Command::Response::ForwardError resp;
    resp.Type = Command::Response::Type::FORWARD_ERROR;
    std::strcpy(resp.HostID.data(), targetHost->HostID.c_str());
    resp.ForwardID = SGX::Net::IP::htons(req->ForwardID);
    resp.Protocol = (Command::AttrType::Proto)SGX::Net::IP::htons(req->Protocol);
    resp.RemotePort = SGX::Net::IP::htons(req->RemotePort);

    // Send response
    auto sent = host->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
    if (!sent)
        throw SGX::Net::Except::HostDisconnectedException("Failed to send ForwardError response");
}
