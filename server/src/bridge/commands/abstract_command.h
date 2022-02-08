#ifndef ABSTRACT_COMMAND_H_
# define ABSTRACT_COMMAND_H_

#include "command_interface.h"
#include "sgxlib/logger/logger.hpp"
#include "../hosts_container.h"

namespace Command
{
    namespace Runner
    {
        class Abstract : public Interface
        {

        public:

            Abstract(SGX::Logger::Interface *, HostsContainer *);
            
            ~Abstract();

            virtual void            Run(Host::Ptr &host) = 0;

        protected:

            SGX::Logger::Interface  *logger;
            HostsContainer          *hostsContainer;
        };
    }
}
#endif //ABSTRACT_COMMAND_H_
