#ifndef BOOST_CLIENT_H_
# define BOOST_CLIENT_H_

#include <boost/asio.hpp>
#include "../client_interface.h"

namespace SGX
{
    namespace Net
    {
        namespace Client
        {

            namespace detail
            {
                template<typename SInterface>
                class BoostBaseClient : public SInterface
                {
                public:

                    BoostBaseClient(const std::string &host, const std::string &port) : host(host), port(port) {}
                    std::string                 GetServerAddress() const { return host + ":" + port; }

                protected:

                    boost::asio::io_service    io_service;
                    std::string                host;
                    std::string                port;
                };
            }


            namespace TCP
            {
                class BoostClient : public detail::BoostBaseClient<TCP::Interface>
                {

                public:
		  
                    BoostClient(const std::string &host, const std::string &port, unsigned int linger_seconds = 5);
                    Socket::TCP::Interface::Ptr Connect();

                private:

                    unsigned int               linger_seconds;
                };
		
            }; //TCP


            namespace UDP
            {
                class BoostClient : UDP::Interface
                {

                public:

                    BoostClient();
                    Socket::UDP::Interface      *Open(const std::string &, const std::string &);

                private:

                    boost::asio::io_service    io_service;
                };
            } //UDP



        }; //CLIENT
    }; //NET
}; //SGX

#endif // BOOST_CLIENT_H_
