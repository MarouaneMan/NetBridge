#ifndef OUTGOING_UDP_FORWARDER_MANAGER_H_
# define OUTGOING_UDP_FORWARDER_MANAGER_H_

#include <vector>
#include "sgxlib/net/socket_interface.h"
#include "outgoing_forwarder_interface.h"
#include "../../main_event_emitter_interface.h"
#include "../../connection_event_emitter_interface.h"
#include "../../logs_event_emitter_interface.h"
#include "../host.h"
#include <vector>
#include <stack>
#include "proto/command.h"

namespace Forwarder
{
    namespace Outgoing
    {
        namespace UDP
        {
            class Manager
            {

            public:

                Manager(
                    MainEventEmitterInterface &,
                    ConnectionEventEmitterInterface	&,
                    LogsEventEmitterInterface &
                );

                void        Run(std::string hostID, std::string loopback, unsigned short udpBridgePort);
                void        DestroyOutgoingForwarders();

            private:

                MainEventEmitterInterface                   &mainEventEmitter;
                ConnectionEventEmitterInterface             &outgoingConnectionEventEmitter;
                LogsEventEmitterInterface                   &logsEmitter;
                std::stack<std::thread>                     threads;
                std::vector<Outgoing::UDP::Interface::Ptr>  outgoingUDPForwarders;
            };
        };
    };
};

#endif //OUTGOING_UDP_FORWARDER_MANAGER_H_