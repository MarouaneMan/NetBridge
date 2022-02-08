#ifndef FORWARDER_PEER_H_
# define FORWARDER_PEER_H_

#include "sgxlib/net/socket_interface.h"
#include "sgxlib/net/client_interface.h"
#include "forwarder_peer_interface.h"

namespace Forwarder
{
    namespace Peer
    {
        class TCPPeer : public Interface
        {
            public:

                TCPPeer(SGX::Net::Socket::TCP::Interface::Ptr);
                TCPPeer(SGX::Net::Client::TCP::Interface::Ptr);
                ~TCPPeer();

                bool            Init();

                // return 0  on failure
                size_t          ReadSome(std::vector<char> &buffer);

                // return false on failure
                bool            WriteAll(const char *data, size_t len_data);

                void            Close();

            private:

                SGX::Net::Socket::TCP::Interface::Ptr        sock;
                SGX::Net::Client::TCP::Interface::Ptr   client;
        };
    };
};

#endif //FORWARDER_TCP_H_