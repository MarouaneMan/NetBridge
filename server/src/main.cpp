#include <iostream>
#include "proto/command.h"
#include "bridge/bridge_core.h"
#include "sgxlib/logger/simple_logger.hpp"
#include "sgxlib/net/boost/server.h"

int	main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "Usage : NetBridgeServer <command_port>" << std::endl;
        return EXIT_FAILURE;
    }

    // Logger
    SGX::Logger::SimpleLogger logger;

    try
    {
        // CommandServer
        SGX::Net::Server::TCP::BoostServer commandServer("0.0.0.0", std::atoi(av[1]));

        // Bridge Core
        BridgeCore	core(
            &logger,
            &commandServer
        );

        // Run
        core.Run();
    }
    catch (std::exception &e)
    {
        logger.Fatal(e.what());
    }

    return EXIT_FAILURE;
}