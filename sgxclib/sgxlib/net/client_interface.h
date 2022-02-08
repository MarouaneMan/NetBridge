#ifndef NET_CLIENT_H_
# define NET_CLIENT_H_

#include <string>
#include <memory>
#include "socket_interface.h"

namespace SGX
{
    namespace Net
    {
        namespace Client
        {
            namespace TCP
            {
                class Interface
                {

                public:

                    using Ptr   = std::unique_ptr<TCP::Interface>;
                    using SPtr  = std::shared_ptr<TCP::Interface>;

                    virtual Socket::TCP::Interface::Ptr	Connect() = 0;
                    virtual std::string				    GetServerAddress() const = 0;
                };

            }; // TCP


            namespace UDP
            {
                class Interface
                {

                public:

                    using Ptr   = std::unique_ptr<UDP::Interface>;
                    using SPtr  = std::shared_ptr<UDP::Interface>;

                    virtual Socket::UDP::Interface  *Open(const std::string &, const std::string &) = 0;
				};
			} // UDP

        }; //CLIENT
    }; //NET
}; //SGX


#endif // NET_CLIENT_H_