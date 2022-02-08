#ifndef INCOMING_UDP_FORWARDER_H_
# define INCOMING_UDP_FORWARDER_H_

#include "../../main_event_emitter_interface.h"
#include "../../logs_event_emitter_interface.h"
#include "../../connection_event_emitter_interface.h"
#include "sgxlib/net/client_interface.h"
#include "proto/command.h"
#include <unordered_map>

namespace Forwarder
{
    namespace UDP
    {
        class IncomingForwarder
        {

        public:
            IncomingForwarder(
                MainEventEmitterInterface &,
                LogsEventEmitterInterface &,
                ConnectionEventEmitterInterface &incomingConnectionEventEmitter
            );
            ~IncomingForwarder();

            void                            Run(Command::Response::UDPForwarderEndPoint, std::string, std::string);

        private:

            using MapSock = std::unordered_map<unsigned short, SGX::Net::Socket::UDP::Interface::SPtr>;

            MainEventEmitterInterface       &mainEventEmitter;
            LogsEventEmitterInterface       &logsEmitter;
            ConnectionEventEmitterInterface &incomingConnectionEventEmitter;
            MapSock                         mapSock;
        };
    };

}

#endif //INCOMING_UDP_FORWARDER_H_