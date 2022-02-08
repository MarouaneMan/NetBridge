#ifndef INCOMING_TCP_FORWARDER_H_
# define INCOMING_TCP_FORWARDER_H_

#include "../../main_event_emitter_interface.h"
#include "../../logs_event_emitter_interface.h"
#include "../../connection_event_emitter_interface.h"
#include "proto/command.h"

namespace Forwarder
{
    namespace TCP
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

            void                            Run(Command::Response::ForwardOrder, std::string);

        private:

            MainEventEmitterInterface       &mainEventEmitter;
            LogsEventEmitterInterface       &logsEmitter;
            ConnectionEventEmitterInterface &incomingConnectionEventEmitter;
        };
    };

}

#endif //INCOMING_TCP_FORWARDER_H_