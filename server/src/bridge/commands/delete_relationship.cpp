#include "delete_relationship.h"
#include "proto/command.h"
#include "sgxlib/struct_caster.hpp"
#include "sgxlib/string.hpp"

Command::Runner::DeleteRelationship::DeleteRelationship(SGX::Logger::Interface *logger, HostsContainer *hostsContainer)
  : Abstract(logger, hostsContainer)
{
}

void     Command::Runner::DeleteRelationship::Run(Host::Ptr &host)
{
    // Parse Request
    Command::Request::DeleteRelationship *req = SGX::StructCaster::Vector::Eq<Command::Request::DeleteRelationship>(host->Buffer);

    // Find target host
    SGX::String::NullTerminateStdArray(req->HostID);
    auto targetHost = hostsContainer->FindHostByID(&req->HostID[0]);

    // Host found
    if (targetHost != nullptr)
    {
        // Destroy relationship
        host->DeleteRelationShip(targetHost->HostID);

        // Send Relationship deleted event to to target host
        Command::Response::RelationshipDeleted resp;
        resp.Type = Command::Response::Type::RELATIONSHIP_DELETED;
        std::strcpy(resp.HostID.data(), host->HostID.c_str());
        targetHost->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));

        logger->Info("Host : " + host->HostID + " deleted " + &req->HostID[0]);
    }
    else
    {
        logger->Warning("Host : " + host->HostID + " tried to delete unknown host : " + &req->HostID[0]);
    }
}
