#ifndef OUTGOING_TCP_FORWARDER_H_
# define OUTGOING_TCP_FORWARDER_H_

#include "outgoing_forwarder_interface.h"
#include "sgxlib/net/server_interface.h"
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
        namespace TCP
        {
            class TCPForwarder : public Outgoing::TCP::Interface
            {

            public:

                struct Info
                {
                    SGX::Net::Socket::TCP::Interface::Ptr	LockSock;
                    Connection::SockInfo::Ptr			SockInfo;
                    Forwarder::Forward					*Forwarder;
                };

                using MapSock = std::unordered_map<unsigned short, std::unique_ptr<Info>>;

                TCPForwarder(
                    SGX::Net::Socket::TCP::Interface *,
                    Host::Ptr,
                    SGX::Net::Server::TCP::Interface::Ptr,
                    MainEventEmitterInterface &,
                    ConnectionEventEmitterInterface	&,
                    LogsEventEmitterInterface &,
                    unsigned short, unsigned short
                );

                ~TCPForwarder();

                void    RunAcceptor();
                void    Destroy();
                void    StartForward(unsigned short, unsigned short);
                void    KillForward(unsigned short);

            private:
                SGX::Net::Socket::TCP::Interface                 *commandServer;
                Host::Ptr                                   targetHost;
                SGX::Net::Server::TCP::Interface::Ptr       server;
                MainEventEmitterInterface                   &mainEventEmitter;
                ConnectionEventEmitterInterface	            &outgoingConnectionEventEmitter;
                LogsEventEmitterInterface                   &logsEmitter;
                std::atomic<bool>                           destroyed;
                MapSock                                     sockMap;
                std::mutex                                  mutex;
                unsigned short								mapped_port;
                unsigned short                              port;
            };
        };
    }

};

#endif //OUTGOING_TCP_FORWARDER_H_
