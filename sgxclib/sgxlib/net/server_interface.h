#ifndef NET_SERVER_INTERFACE_H_
#define NET_SERVER_INTERFACE_H_

#include <vector>
#include <memory>
#include "socket_interface.h"

namespace SGX
{
    namespace Net
    {
        namespace Server
        {
            namespace TCP
            {
                class Interface
                {

                public:

                    using Ptr = std::unique_ptr<TCP::Interface>;
                    using SPtr = std::shared_ptr<TCP::Interface>;

                    virtual                             ~Interface() {};
                    virtual void					    Init() = 0;
                    virtual Socket::TCP::Interface::Ptr	Accept() = 0;
                    virtual void                        Destroy() = 0;
                    virtual std::string				    GetBoundAddress() const = 0;
                    virtual unsigned short              GetPort() const = 0;
                    virtual std::string                 GetBoundIP() const = 0;
                };

            }; //TCP


            namespace UDP
            {
                class Interface
                {

                public:

                    using Ptr = std::unique_ptr<UDP::Interface>;
                    using SPtr = std::shared_ptr<UDP::Interface>;

                    virtual                             ~Interface() {};
                    virtual void					    Init() = 0;
                    virtual void                        Destroy() = 0;
                    virtual size_t                      ReadSome(std::vector<char> &) = 0;
                    virtual std::string				    GetBoundAddress() const = 0;
                    virtual unsigned short              GetPort() const = 0;
                    virtual std::string                 GetBoundIP() const = 0;
                };
            };


        }; //SERVER
    }; //NET
}; //SGX

#endif // SERVER_INTERFACE_H_