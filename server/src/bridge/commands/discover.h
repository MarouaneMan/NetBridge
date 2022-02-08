#ifndef DISCOVER_COMMAND_H_
# define DISCOVER_COMMAND_H_

#include "abstract_command.h"

namespace Command
{
    namespace Runner
    {
        class Discover : public Abstract
        {

        public:

            Discover(SGX::Logger::Interface *, HostsContainer *);
            
            void            Run(Host::Ptr &host);
            static void     HostFoundEvent(SGX::Logger::Interface *, Host::Ptr &, Host::Ptr &);
            static void     DiscoveredEvent(SGX::Logger::Interface *, Host::Ptr &, Host::Ptr &);

        private:
            
            void            sendUDPEndpoint(SGX::Logger::Interface *, Host::Ptr &, Host::Ptr &, unsigned short);

        };
    }
}
#endif //REGISTER_COMMAND_H_
