#ifndef COMMAND_INTERFACE_H_
# define COMMAND_INTERFACE_H_

#include "../host.h"

namespace Command
{
    namespace Runner
    {
        class Interface
        {

        public:

	    virtual void Run(Host::Ptr &host) = 0;

        };
    };
};

#endif // COMMAND_INTERFACE_H_
