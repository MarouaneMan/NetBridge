#ifndef DELETE_RELATION_SHIP_COMMAND_H_
# define DELETE_RELATION_SHIP_COMMAND_H_

#include "abstract_command.h"

namespace Command
{
    namespace Runner
    {
        class DeleteRelationship : public Abstract
        {

        public:

            DeleteRelationship(SGX::Logger::Interface *, HostsContainer *);
            
            void        Run(Host::Ptr &host);
        };
    }
}
#endif //DELETE_RELATION_SHIP_COMMAND_H_
