#ifndef COMMAND_PROCESSOR_H_
# define COMMAND_PROCESSOR_H_

#include "sgxlib/logger/logger.hpp"
#include "sgxlib/net/server_interface.h"
#include "sgxlib/callback_registrator.hpp"
#include "host.h"
#include <unordered_map>
#include <shared_mutex>
#include "hosts_container.h"

class CommandProcessor
{

public:

    CommandProcessor(SGX::Logger::Interface *logger);

    ~CommandProcessor();

    void                Process(SGX::Net::Socket::TCP::Interface::Ptr conn);

private:

    void                notifyHostDied(Host::Ptr &);

    SGX::Logger::Interface                  *logger;
    SGX::Net::Server::TCP::Interface        *commandServer;
    HostsContainer                          hostsContainer;
    
    // CallbackRegistrator
    SGX::Callback::Registrator<
        int,                        // KeyType
        void,                       // ReturnType
        Host::Ptr &>                // Callback Args
        callbackRegistrator;
};

#endif //COMMAND_PROCESSOR_H_
