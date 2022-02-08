#ifndef FORWARDER_TCP_H_
# define FORWARDER_TCP_H_

#include "forwarder_interface.h"
#include "sgxlib/net/server_interface.h"
#include "sgxlib/logger/logger.hpp"
#include <atomic>

namespace Forwarder
{
    namespace TCP
    {
        class Forwarder : public TCP::Interface
        {

        public:

            Forwarder(
                SGX::Net::Server::TCP::Interface::Ptr,
                SGX::Logger::Interface *logger
            );

            unsigned short  Init();
            void            Run(const std::string &, const std::string &, unsigned short, unsigned long);

        private:
            
            void            forward(SGX::Net::Socket::TCP::Interface*, SGX::Net::Socket::TCP::Interface *, unsigned long);
            void            forwardTimeoutChecker(const std::string &);

            SGX::Net::Server::TCP::Interface::Ptr   server;
            SGX::Net::Socket::TCP::Interface::Ptr   firstPeer;
            SGX::Net::Socket::TCP::Interface::Ptr   secondPeer;
            SGX::Logger::Interface                  *logger;
            std::atomic<bool>                       timeout;

        };
    }
};

#endif //FORWARDER_TCP_H_
