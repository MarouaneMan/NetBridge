#ifndef FORWARDER_INTERFACE_PEER_H_
# define FORWARDER_INTERFACE_PEER_H_

#include <memory>
#include <vector>

namespace Forwarder
{
    namespace Peer
    {
        class Interface
        {

        public:

            using Ptr = std::unique_ptr<Interface>;

            virtual         ~Interface() {};
            
            virtual bool    Init() = 0;

            // return 0  on failure
            virtual size_t  ReadSome(std::vector<char> &buffer) = 0;

            // return false on failure
            virtual bool    WriteAll(const char *data, size_t len_data) = 0;

            virtual void    Close() = 0;
        };
        
    };
};

#endif //FORWARDER_INTERFACE_PEER_H_