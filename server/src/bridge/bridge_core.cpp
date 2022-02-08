#include "bridge_core.h"
#include <memory>
#include <thread>
#include "proto/command.h"
#include "sgxlib/net/exception.hpp"

BridgeCore::BridgeCore(
    SGX::Logger::Interface *logger,
    SGX::Net::Server::TCP::Interface *commandServer
) :
    logger(logger),
    commandServer(commandServer)
{
}

BridgeCore::~BridgeCore()
{
}

void	BridgeCore::Run()
{
    try
    {
        // Start command server
        commandServer->Init();

        // Command processor
        CommandProcessor commandProcessor(logger);

        logger->Info("Listening on : " + commandServer->GetBoundAddress());

        // Accept new connections
        while (SGX::Net::Socket::TCP::Interface::Ptr sock = commandServer->Accept())
        {
            logger->Info("New connection from : " + sock->GetRemoteIP());

            // Spawn thread
            std::thread(&CommandProcessor::Process, &commandProcessor, std::move(sock)).detach();
        }
    }
    catch (SGX::Net::Except::InitException &e)
    {
        logger->Error(std::string("CommandServer failed to start : ") + e.what());
    }
    catch (SGX::Net::Except::AcceptException &e)
    {
        logger->Error(std::string("Accept Failed : ") + e.what());
    }
    catch (std::exception &e)
    {
        logger->Error(std::string("CommandServer unexpectedly stopped : ") + e.what());
    }
}
