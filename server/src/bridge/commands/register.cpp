#include "register.h"
#include "proto/command.h"
#include "sgxlib/struct_caster.hpp"
#include "sgxlib/string.hpp"
#include "sgxlib/net/ip.h"
#include "discover.h"

Command::Runner::Register::Register(SGX::Logger::Interface *logger, HostsContainer *hostsContainer)
    : Abstract(logger, hostsContainer)
{
}

void     Command::Runner::Register::Run(Host::Ptr &host)
{
    // Parse Request
    Command::Request::Register *req = SGX::StructCaster::Vector::Eq<Command::Request::Register>(host->Buffer);

    // Copy attributes
    SGX::String::SafeCopyStdArrayToString(host->Hostname, req->Hostname);
    SGX::String::SafeCopyStdArrayToString(host->HostID, req->HostID);
    SGX::String::SafeCopyStdArrayToString(host->LocalIP, req->LocalIP);
    
    // Check readSomeSize for bad jokes
    host->ReadSomeSize = SGX::Net::IP::ntohl(static_cast<uint32_t>(req->ReadSomeSize));
    if (host->ReadSomeSize > 1024 * 1024 * 6)
        throw std::runtime_error("Bad joke, readSomeSize = " + std::to_string(req->ReadSomeSize));
    
    // Save host
    hostsContainer->AddHost(host);

    logger->Info("New host registered : " + host->HostID + ", " + host->Hostname + ", " + host->GetRemoteIP());

    // Notify friends that host is up again
    hostsContainer->Foreach([&](Host::Ptr &h)
    {
        h->ForeachRelationShip([&](const std::string &friendHostID)
        {
            if (friendHostID == host->HostID)
            {
                // Host Up response
                Command::Response::HostUp resp;
                resp.Type = Command::Response::Type::HOST_UP;
                std::strcpy(resp.HostID.data(), host->HostID.c_str());

                // Send response
                h->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
            }
        });
    });

}
