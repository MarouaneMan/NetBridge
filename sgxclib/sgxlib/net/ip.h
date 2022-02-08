#ifndef SGX_NET_IP_H_
#define SGX_NET_IP_H_

#include "../string.hpp"
#include <vector>

namespace SGX
{
    namespace Net
    {
        namespace IP
        {
            // String vector x.x.x.x to uint32_t ip
            unsigned int VectorToIPV4(const std::vector<std::string> &vec);

            // Get Next IP from range (from_address,to_address)
            std::string GetNextIP(const std::string &from_address, const std::string &to_address);

            // Get machine Hostname
            std::string GetHostname();

            // Host to Network short
            unsigned short htons(unsigned short a);

            // Network to Host short
            unsigned short ntohs(unsigned short a);

            // Host to Network long
            unsigned long htonl(unsigned long a);

            // Network to Host long
            unsigned long ntohl(unsigned long a);

        };
    };
};

#endif //SGX_NET_IP_H_