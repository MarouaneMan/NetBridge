#ifndef FORWARD_COMMAND_H_
# define FORWARD_COMMAND_H_

#include "abstract_command.h"
#include "proto/command.h"

namespace Command
{
    namespace Runner
    {
        class Forward : public Abstract
        {

        public:

            Forward(SGX::Logger::Interface *, HostsContainer *);
            
            void        Run(Host::Ptr &host);

        private:
            
            void        forwardTCP(Host::Ptr &, Host::Ptr &, Request::Forward *);
            void        sendForwardAcceptedToHost(Host::Ptr &, Host::Ptr &, Request::Forward *, unsigned short);
            bool        sendForwardOrderToTargetHost(Host::Ptr &, Host::Ptr &, Request::Forward *, unsigned short);
            void        sendForwardErrorToHost(Host::Ptr &, Host::Ptr &, Request::Forward *);
        };
    }
}
#endif //FORWARD_COMMAND_H_
