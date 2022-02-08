#include "command_processor.h"
#include "sgxlib/net/ip.h"
#include "sgxlib/net/exception.hpp"
#include "sgxlib/struct_caster.hpp"
#include "sgxlib/string.hpp"
#include "proto/command.h"
#include "commands/register.h"
#include "commands/discover.h"
#include "commands/delete_relationship.h"
#include "commands/forward.h"

CommandProcessor::CommandProcessor(SGX::Logger::Interface *logger) : logger(logger)
{
    // Register command
    callbackRegistrator.Register(new Command::Runner::Register(logger, &hostsContainer),
        Command::Request::Type::REGISTER,
        &Command::Runner::Register::Run
    );

    // Discover command
    callbackRegistrator.Register(new Command::Runner::Discover(logger, &hostsContainer),
        Command::Request::Type::DISCOVER_HOST,
        &Command::Runner::Discover::Run
    );

    // Delete relationship command
    callbackRegistrator.Register(new Command::Runner::DeleteRelationship(logger, &hostsContainer),
        Command::Request::Type::DELETE_RELATIONSHIP,
        &Command::Runner::DeleteRelationship::Run
    );

    // Forward command
    callbackRegistrator.Register(new Command::Runner::Forward(logger, &hostsContainer),
        Command::Request::Type::FORWARD,
        &Command::Runner::Forward::Run
    );
}

CommandProcessor::~CommandProcessor()
{
}

void	CommandProcessor::Process(SGX::Net::Socket::TCP::Interface::Ptr conn)
{
    // New Host
    Host::Ptr host = std::make_shared<Host>(conn);
    
    while (host->ReadMessage(512))
    {
        try
        {
            logger->Debug(std::string("Received new message : ") + std::to_string(host->Buffer.size()));

            // Accept BaseRequest
            Command::Request::BaseRequest* request = SGX::StructCaster::Vector::Gte<Command::Request::BaseRequest>(host->Buffer);

            // Call Requested command
            callbackRegistrator.Call(request->Type, host);
        }
        catch (SGX::StructCaster::Exception::BadSizeException &e)
        {
            logger->Warning("HostIP : " + host->GetRemoteIP() + " : " + e.what());
        }
        catch (SGX::Callback::Exception::UnknownCallbackException &)
        {
            logger->Warning("HostIP : " + host->GetRemoteIP() + " : requested unknown command");
        }
        catch (SGX::Net::Except::HostDisconnectedException &e)
        {
            logger->Warning("HostIP : " + host->GetRemoteIP() + " disconnected : " + e.what());
            break;
        }
        catch (std::exception &e)
        {
            logger->Error("HostIP : " + host->GetRemoteIP() + " unknown exception : " + e.what());
        }
    }

    // Host Died, Notify Friends
    notifyHostDied(host);

    // Delete Host
    hostsContainer.DeleteHost(host);

    logger->Warning("HostIP : " + host->GetRemoteIP() + " : disconnected");

    // DEBUG
    logger->Debug("HostsContainer size : " + std::to_string(hostsContainer.Size()) + " ______________");
}

void	CommandProcessor::notifyHostDied(Host::Ptr &host)
{
    hostsContainer.Foreach([&](Host::Ptr &h)
    {
        //Send events to friends
        h->ForeachRelationShip([&](const std::string &friendHostID)
        {
            if (friendHostID == host->HostID)
            {
                // Host died response
                Command::Response::HostDied resp;
                resp.Type = Command::Response::Type::HOST_DIED;
                std::strcpy(resp.HostID.data(), host->HostID.c_str());

                // Send response
                h->WriteMessage(reinterpret_cast<const char*>(&resp), sizeof(resp));
            }
        });
    });
}
