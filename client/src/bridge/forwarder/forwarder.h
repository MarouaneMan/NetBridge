#ifndef FORWARDER_H_
# define FORWARDER_H_

#include "forwarder_peer_interface.h"
#include "../connection.h"
#include "../../logs_event_emitter_interface.h"

namespace Forwarder
{
    class Forward
    {

    public:

        Forward(
            Connection::SockInfo::Ptr,
            Peer::Interface::Ptr src,
            Peer::Interface::Ptr dst,
            LogsEventEmitterInterface &
        );
        
        void                        Run();
		void						Close();

    private:

        void                        forward(Peer::Interface *, Peer::Interface *, unsigned long);

        Connection::SockInfo::Ptr   sockInfo;
        Peer::Interface::Ptr        src;
        Peer::Interface::Ptr        dst;
        LogsEventEmitterInterface   &logsEmitter;
    };
}

#endif //FORWARDER_TCP_H_