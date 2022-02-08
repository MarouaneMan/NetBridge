#ifndef BRIDGE_CORE_H_
# define BRIDGE_CORE_H_

#include "command_processor.h"

class BridgeCore
{

public:

    BridgeCore(
        SGX::Logger::Interface *logger,
        SGX::Net::Server::TCP::Interface *commandServer
    );

    ~BridgeCore();

    void                                Run();

private:

    SGX::Net::Server::TCP::Interface    *commandServer;
    SGX::Logger::Interface              *logger;
};

#endif // BRIDGE_CORE_H_