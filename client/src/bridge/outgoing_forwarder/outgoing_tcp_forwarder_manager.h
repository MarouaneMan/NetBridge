#ifndef OUTGOING_TCP_FORWARDER_MANAGER_H_
# define OUTGOING_TCP_FORWARDER_MANAGER_H_

#include <vector>
#include "sgxlib/net/socket_interface.h"
#include "outgoing_forwarder_interface.h"
#include "../../main_event_emitter_interface.h"
#include "../../connection_event_emitter_interface.h"
#include "../../logs_event_emitter_interface.h"
#include "../host.h"
#include <unordered_map>
#include <stack>
#include "proto/command.h"

namespace Forwarder
{
    namespace Outgoing
    {
        namespace TCP
        {
            class Manager
            {

            public:

                Manager(
                    const std::string &,
                    MainEventEmitterInterface &,
                    ConnectionEventEmitterInterface	&,
                    LogsEventEmitterInterface &
                );

                void    SpawnOutgoingForwarder(SGX::Net::Socket::TCP::Interface *, Host::Ptr);
                void    DestroyOutgoingForwarders();
                void    StartForward(Command::AttrType::Proto, unsigned short remotePort, unsigned short bridgePort, unsigned short forwardID);
                void    KillForwardSock(Command::AttrType::Proto, unsigned short remotePort, unsigned short forwardID);

            private:

                using OutgoingForwardersMap = std::unordered_map<unsigned short, Outgoing::TCP::Interface::Ptr>;

                std::string                         loopback;
                OutgoingForwardersMap               outgoingTcpForwarders;
                MainEventEmitterInterface           &mainEventEmitter;
                ConnectionEventEmitterInterface     &outgoingConnectionEventEmitter;
                LogsEventEmitterInterface           &logsEmitter;
                std::stack<std::thread>             threads;
            };
        };
    };
};

#endif //OUTGOING_FORWARDER_MANAGER_H_