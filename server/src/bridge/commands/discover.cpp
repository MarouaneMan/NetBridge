#include "discover.h"
#include "proto/command.h"
#include "sgxlib/struct_caster.hpp"
#include "sgxlib/string.hpp"
#include "sgxlib/net/exception.hpp"
#include "../forwarder/forwarder_udp.h"
#include "sgxlib/net/boost/server.h"
#include "sgxlib/net/ip.h"
#include <cstring>
#include <thread>


Command::Runner::Discover::Discover(SGX::Logger::Interface *logger, HostsContainer *hostsContainer)
  : Abstract(logger, hostsContainer)
{
}

void     Command::Runner::Discover::Run(Host::Ptr &host)
{
    // Parse Request
    Command::Request::DiscoverHost *req = SGX::StructCaster::Vector::Eq<Command::Request::DiscoverHost>(host->Buffer);

    SGX::String::NullTerminateStdArray(req->HostID);
    
    // Check ui bypass
    if (std::strcmp(host->HostID.c_str(), &req->HostID[0]) == 0)
    {
      logger->Warning("HostID : " + host->HostID + " tried to discover itself");
      return ;
    }
    
    // Find target host
    auto targetHost = hostsContainer->FindHostByID(&req->HostID[0]);

    // Host found
    if (targetHost != nullptr)
    {
      // Add relationship
      host->AddRelationShip(targetHost->HostID);

      // Add relationship to targetHost (optional can be removed)
      targetHost->AddRelationShip(host->HostID);
      
      // Send HostFound/HostDiscovered Events
      HostFoundEvent(logger, host, targetHost);
      DiscoveredEvent(logger, host, targetHost);

      // If UDP Enabled spawn new udp forwarder
      if (req->UDPEnabled)
      {
          // Allocate UDP Forwarder
          Forwarder::UDP::Forwarder *forwarder = new Forwarder::UDP::Forwarder(
              std::unique_ptr<SGX::Net::Server::TCP::Interface>(new SGX::Net::Server::TCP::BoostServer("0.0.0.0", 0)),
              host,
              targetHost,
              logger
          );
          unsigned short udpForwarderPort = forwarder->Init();
          if (udpForwarderPort)
          {
              std::thread(&Forwarder::UDP::Forwarder::Run, forwarder).detach();
              sendUDPEndpoint(logger, host, targetHost, udpForwarderPort);
              sendUDPEndpoint(logger, targetHost, host, udpForwarderPort);
          }
          else
          {
              logger->Warning("Failed to allocate new UDP Forwarder for : " + host->HostID);
          }
      }
    }
    // Host not found
    else
    {
        // Save the relationship even if the targetHost isn't found
        // Reason : may connect later
        // Security : limit the number of relationships to avoid evil discover requests
        if (host->RelationShipsSize() < 500)
           host->AddRelationShip(&req->HostID[0]);
	
        // Host not found response
        Command::Response::HostNotFound resp;
        resp.Type = Command::Response::Type::HOST_NOTFOUND;
        std::strcpy(resp.HostID.data(), &req->HostID[0]);

        // Send response
        auto sent = host->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
        if (!sent)
            throw SGX::Net::Except::HostDisconnectedException("Failed to send HostNotFound response");

        logger->Warning("Host : " + host->HostID + " tried to discover unknown host " + &req->HostID[0]);
    }
}

void	    Command::Runner::Discover::HostFoundEvent(SGX::Logger::Interface *logger, Host::Ptr &host, Host::Ptr &targetHost)
{
    // Host found response
    Command::Response::HostFound resp;
    resp.Type = Command::Response::Type::HOST_FOUND;
    std::strcpy(resp.HostID.data(), targetHost->HostID.c_str());
    std::strcpy(resp.Hostname.data(), targetHost->Hostname.c_str());
    std::strcpy(resp.LocalIP.data(), targetHost->LocalIP.c_str());
    auto sent = host->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
    if (!sent)
      throw SGX::Net::Except::HostDisconnectedException("Failed to send HostNotFound response");
    
    logger->Info("Host : " + targetHost->HostID + " discovered by " + host->HostID);
}

void	    Command::Runner::Discover::DiscoveredEvent(SGX::Logger::Interface *logger, Host::Ptr &host, Host::Ptr &targetHost)
{
    //Send discovered event to target host
    Command::Response::CurrentHostDiscovered discovered_event;
    discovered_event.Type = Command::Response::Type::CURRENT_HOST_DISCOVERED;
    std::strcpy(discovered_event.HostID.data(), host->HostID.c_str());
    std::strcpy(discovered_event.Hostname.data(), host->Hostname.c_str());
    std::strcpy(discovered_event.LocalIP.data(), host->LocalIP.c_str());
    auto sent = targetHost->WriteMessage(reinterpret_cast<const char*>(&discovered_event), sizeof(discovered_event));
    if (!sent)
      logger->Warning("Failed to send discovered event to : " + targetHost->HostID);
}

void        Command::Runner::Discover::sendUDPEndpoint(SGX::Logger::Interface *logger, Host::Ptr &host, Host::Ptr &targetHost, unsigned short udpForwarderPort)
{
    // Host found response
    Command::Response::UDPForwarderEndPoint resp;
    resp.Type = Command::Response::Type::UDP_FORWARDER_ENDPOINT;
    std::strcpy(resp.HostID.data(), targetHost->HostID.c_str());
    resp.Port = SGX::Net::IP::htons(udpForwarderPort);
    auto sent = host->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
    if (!sent)
        logger->Warning("Failed to send UDPForwarderEndpoint to " + host->HostID);
}