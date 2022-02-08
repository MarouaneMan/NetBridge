#ifndef SOCKET_INTERFACE_H_
# define SOCKET_INTERFACE_H_

#include <vector>
#include <memory>
#include <mutex>

namespace SGX
{
    namespace Net
    {
        namespace Socket
        {
            namespace UDP
            {
                class Interface 
                {

                public:

                    using Ptr = std::unique_ptr<UDP::Interface>;
                    using SPtr = std::shared_ptr<UDP::Interface>;

                    virtual             ~Interface() {};
                    virtual std::string	GetRemoteIP() = 0;
                    virtual std::string	GetLocalIP() = 0;
                    virtual size_t      ReadSome(std::vector<char> &) = 0;
                    virtual bool        WriteAll(const char *data, size_t len_data) = 0;
                    virtual void        ForceClose() = 0;
                };

            };

            namespace TCP
            {
                class Interface : public UDP::Interface
                {

                public:

                    using Ptr = std::unique_ptr<TCP::Interface>;
                    using SPtr = std::shared_ptr<TCP::Interface>;

                    virtual             ~Interface() {};
                    virtual bool		ReadMessage(std::vector<char> &, size_t maxSize) = 0;
                    virtual bool		WriteMessage(const char *data, size_t len_data) = 0;
                    virtual bool        WriteMessageThSafe(const char *data, size_t len_data) = 0;
                    virtual bool        ReadAll(char *buffer, size_t buff_len) = 0;
                    virtual void        Linger(unsigned int) = 0;
                    virtual void        KeepAlive(bool) = 0;
                    virtual void        TcpNoDelay(bool) = 0;
                };
            };

            

            
       
        } // SOCKET
    } // NET
} // SGX

#endif // SOCKET_INTERFACE_H_
