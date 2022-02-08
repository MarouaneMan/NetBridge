#ifndef FORWARDER_UDP_H_
# define FORWARDER_UDP_H_

#include "forwarder_interface.h"
#include "sgxlib/net/server_interface.h"
#include "sgxlib/logger/logger.hpp"
#include "../host.h"
#include <atomic>

namespace Forwarder
{
    namespace UDP
    {
        class Forwarder
        {

        public:

            Forwarder(
                SGX::Net::Server::TCP::Interface::Ptr,
                Host::Ptr,
                Host::Ptr,
                SGX::Logger::Interface *
            );

            unsigned short  Init();
            void            Run();

        private:

            void            forward();
            void            forwardTimeoutChecker();

            Host::Ptr                               host;
            Host::Ptr                               targetHost;
            SGX::Net::Server::TCP::Interface::Ptr   server;
            SGX::Net::Socket::TCP::Interface::Ptr   hostPeer;
            SGX::Net::Socket::TCP::Interface::Ptr   targetPeer;
            SGX::Logger::Interface                  *logger;
            std::atomic<bool>                       timeout;
        };
    }
};

#endif //FORWARDER_TCP_H_
