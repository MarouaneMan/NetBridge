#ifndef SOCKET_H_
# define SOCKET_H_

#include <vector>
#include <boost/asio.hpp>
#include <utility>
#include <mutex>
#include "../socket_interface.h"

namespace SGX
{
    namespace Net
    {
        namespace Socket
        {

            // TCP Message Representation
            // --------------------------------------------
            // | size_t message_size | data[message_size] |
            // --------------------------------------------

            namespace detail
            {

                template<typename SInterface, typename SocketType, typename EndpointType>
                class BoostBaseSocket : public SInterface
                {
                public:

                    BoostBaseSocket(SocketType Socket) : bsocket(std::move(Socket))
                    {
                    }

                    std::string		GetRemoteIP()
                    {
                        boost::system::error_code ec;
                        EndpointType endpoint = bsocket.remote_endpoint(ec);
                        if (ec)
                            return "illegal call of GetRemoteIP() on closed socket";
                        return endpoint.address().to_string();
                    }

                    std::string		GetLocalIP()
                    {
                        boost::system::error_code ec;
                        EndpointType endpoint = bsocket.local_endpoint(ec);
                        if (ec)
                            return "illegal call of GetLocalIP() on closed socket";
                        return endpoint.address().to_string();
                    }

                    SocketType		&GetSock()
                    {
                        return bsocket;
                    }

                    void                ForceClose()
                    {
                        try
                        {
                            bsocket.shutdown(SocketType::shutdown_both);
                            bsocket.close();
                        }
                        catch (...) { /*Ignored*/ }
                    }

                protected:

                    SocketType	bsocket;
                    std::mutex  writeLock;
                };
            }


            namespace TCP
            {
                class BoostSocket : public detail::BoostBaseSocket<TCP::Interface, boost::asio::ip::tcp::socket, boost::asio::ip::tcp::endpoint>
                {

                public:

                    BoostSocket(boost::asio::ip::tcp::socket Socket) : BoostBaseSocket(std::move(Socket))
                    {
                    }

                    ~BoostSocket()
                    {
                        ForceClose();
                    }

                    bool			ReadMessage(std::vector<char> &buffer, size_t maxSize)
                    {
                        try
                        {
                            // Read Message Size
                            size_t	messageSize;
                            boost::asio::read(bsocket, boost::asio::buffer(&messageSize, sizeof(size_t)));

                            // Endianness
                            messageSize = ntohl(static_cast<u_long>(messageSize));

                            // Silently ignore bad people
                            if (messageSize > maxSize)
                                return false;

                            // Read Message
                            buffer.resize(messageSize);
                            boost::asio::read(bsocket, boost::asio::buffer(&buffer[0], messageSize));

                            return true;
                        }
                        catch (std::exception &)
                        {
                            return false;
                        }
                    }

                    // Read one or more bytes, return 0 on error, or nb read bytes
                    size_t          ReadSome(std::vector<char> &buffer)
                    {
                        try
                        {
                            return bsocket.read_some(boost::asio::buffer(buffer));
                        }
                        catch (std::exception &)
                        {
                            return 0;
                        }
                    }
                    
                    bool        ReadAll(char *buffer, size_t buff_len)
                    {
                        try
                        {
                            // Read data
                            boost::asio::read(bsocket, boost::asio::buffer(buffer, buff_len));

                            return true;
                        }
                        catch (...)
                        {
                            return false;
                        }
                    }

                    bool            WriteAll(const char *data, size_t len_data)
                    {
                        try
                        {
                            // Write data
                            boost::asio::write(bsocket, boost::asio::buffer(data, len_data));

                            return true;
                        }
                        catch (...)
                        {
                            return false;
                        }
                    }

                    bool			WriteMessage(const char *data, size_t len_data)
                    {
                        try
                        {
                            // Endianess
                            size_t messageSize = htonl(static_cast<u_long>(len_data));

                            // Write message size 
                            boost::asio::write(bsocket, boost::asio::buffer(&messageSize, sizeof(size_t)));

                            // Write message data
                            boost::asio::write(bsocket, boost::asio::buffer(data, len_data));

                            return true;
                        }
                        catch (std::exception &)
                        {
                            return false;
                        }
                    }

                    bool        WriteMessageThSafe(const char *data, size_t len_data)
                    {
                        // Lock writer
                        std::lock_guard<std::mutex> lock(writeLock);

                        return WriteMessage(data, len_data);
                    }

                    void                Linger(unsigned int seconds)
                    {
                        if (seconds > 0)
                            bsocket.set_option(boost::asio::socket_base::linger(true, seconds));
                        else
                            bsocket.set_option(boost::asio::socket_base::linger(false, 0));
                    }
                    void                TcpNoDelay(bool stat)
                    {
                        bsocket.set_option(boost::asio::ip::tcp::no_delay(stat));
                    }

                    void                KeepAlive(bool option)
                    {
                        bsocket.set_option(boost::asio::socket_base::keep_alive(option));
                    }

                };
            } // TCP


            namespace UDP
            {
                class BoostSocket : public detail::BoostBaseSocket<UDP::Interface, boost::asio::ip::udp::socket, boost::asio::ip::udp::endpoint>
                {

                public:

                    BoostSocket(boost::asio::ip::udp::socket Socket) : BoostBaseSocket(std::move(Socket)) {}
                    BoostSocket(boost::asio::ip::udp::socket Socket, boost::asio::ip::udp::endpoint targetEndpoint)
                        : BoostBaseSocket(std::move(Socket)), targetEndpoint(targetEndpoint) {}

                    ~BoostSocket()
                    {
                        ForceClose();
                    }


                    // Read one or more bytes, return 0 on error, or nb read bytes
                    size_t          ReadSome(std::vector<char> &buffer) noexcept
                    {
                        try
                        {
                            return  bsocket.receive_from(boost::asio::buffer(buffer), targetEndpoint);
                        }
                        catch (std::exception &)
                        {
                            return 0;
                        }
                    }

                    bool            WriteAll(const char *data, size_t len_data)
                    {
                        try
                        {
                            bsocket.send_to(boost::asio::buffer(data, len_data), targetEndpoint);
                            return true;
                        }
                        catch (...)
                        {
                            return false;
                        }
                    }

                private:

                    boost::asio::ip::udp::endpoint  targetEndpoint;
                };
            } // UDP

        }; //SOCKET
    }; //NET
}; //SGX

#endif // CONNECTION_H_
