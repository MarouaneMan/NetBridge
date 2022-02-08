#ifndef BOOST_SERVER_H_
# define BOOST_SERVER_H_

#include <boost/asio.hpp>
#include "../server_interface.h"

namespace SGX
{
    namespace Net
    {
        namespace Server
        {

            namespace detail
            {
                template<typename SInterface>
                class BoostBaseServer : public SInterface
                {

                public:

                    BoostBaseServer(const std::string &address, unsigned int port) : address(address), port(port) {}

                    std::string                     GetBoundAddress() const { return GetBoundIP() + ":" + std::to_string(GetPort()); }
                    unsigned short                  GetPort() const { return port; }
                    std::string                     GetBoundIP() const { return address; }

                protected:

                    boost::asio::io_service         io_service;
                    std::string                     address;
                    unsigned short                  port;
                };
            }

            namespace TCP
            {
                class BoostServer : public detail::BoostBaseServer<TCP::Interface>
                {

                public:

                    BoostServer(const std::string &address, unsigned int port, unsigned int linger_seconds = 5);
                    ~BoostServer();

                    void					        Init();
                    Socket::TCP::Interface::Ptr     Accept();
                    void                            Destroy();

                private:

                    boost::asio::ip::tcp::acceptor  *acceptor;
                    unsigned int                    linger_seconds;
                };

            }; //TCP



            namespace UDP
            {
                class BoostServer : public detail::BoostBaseServer<UDP::Interface>
                {

                public:

                    BoostServer(const std::string &address, unsigned int port);

                    void					    Init();
                    size_t                      ReadSome(std::vector<char> &);
                    void                        Destroy();

                private:

                    Socket::UDP::Interface::Ptr bsocket;
                };

            } // UDP

        }; //SERVER
    }; //NET
}; //SGX


#endif //BOOST_SERVER_H_
