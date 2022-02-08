#ifndef NET_EXCEPTION_H_
#define NET_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace SGX
{
    namespace Net
    {
        namespace Except
        {
            // Init exception
            class InitException : public std::runtime_error
            {
            public:
                InitException(const char *what) : std::runtime_error(what) {}
            };

            // Accept Exception
            class AcceptException : public std::runtime_error
            {
            public:
                AcceptException(const char *what) : std::runtime_error(what) {}
            };

            // Connection Exception
            class ConnectException : public std::runtime_error
            {
            public:
                ConnectException(const char *what) : std::runtime_error(what) {}
            };

            // Open Exception
            class OpenException : public std::runtime_error
            {
            public:
                OpenException(const char *what) : std::runtime_error(what) {}
            };

            // Host Disconnected
            class HostDisconnectedException : public std::runtime_error
            {
            public:
                HostDisconnectedException(const char *what) : std::runtime_error(what) {}
            };

        }; //Except
    }; //NET
}; //SGX


#endif // NET_EXCEPTION_H_