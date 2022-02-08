#ifndef REGISTER_COMMAND_H_
# define REGISTER_COMMAND_H_

#include "abstract_command.h"

namespace Command
{
    namespace Runner
    {
        class Register : public Abstract
        {

        public:

            Register(SGX::Logger::Interface *, HostsContainer *);
            
            void        Run(Host::Ptr &host);
        };
    }
}
#endif //REGISTER_COMMAND_H_
