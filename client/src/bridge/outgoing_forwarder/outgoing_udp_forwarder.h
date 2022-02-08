#ifndef OUTGOING_UDP_FORWARDER_H_
# define OUTGOING_UDP_FORWARDER_H_

#include "outgoing_forwarder_interface.h"
#include "sgxlib/net/server_interface.h"
#include "sgxlib/net/client_interface.h"
#include "../../main_event_emitter_interface.h"
#include "../../connection_event_emitter_interface.h"
#include "../../logs_event_emitter_interface.h"
#include <memory>
#include <atomic>
#include <stack>
#include <mutex>
#include "../host.h"
#include "../connection.h"
#include "../forwarder/forwarder.h"
#include <unordered_map>

namespace Forwarder
{
    namespace Outgoing
    {
        namespace UDP
        {
            class UDPForwarder : public Outgoing::UDP::Interface
            {

            public:

                UDPForwarder(
                    SGX::Net::Socket::TCP::Interface *,
                    SGX::Net::Server::UDP::Interface::Ptr,
                    MainEventEmitterInterface &,
                    ConnectionEventEmitterInterface	&,
                    LogsEventEmitterInterface &,
                    unsigned short, unsigned short
                );

                ~UDPForwarder();

                void    Run();
                void    Close();

            private:
                SGX::Net::Socket::TCP::Interface            *udpBridgeSock;
                SGX::Net::Server::UDP::Interface::Ptr       server;
                MainEventEmitterInterface                   &mainEventEmitter;
                ConnectionEventEmitterInterface	            &outgoingConnectionEventEmitter;
                LogsEventEmitterInterface                   &logsEmitter;
                std::atomic<bool>                           destroyed;
                std::mutex                                  mutex;
                unsigned short								mapped_port;
                unsigned short                              port;
            };
        };
    }

};

#endif //OUTGOING_TCP_FORWARDER_H_
