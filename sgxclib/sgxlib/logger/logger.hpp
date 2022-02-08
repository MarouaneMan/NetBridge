#ifndef SGX_LOGGER_INTERACE_H_
# define SGX_LOGGER_INTERACE_H_

#include <string>

namespace SGX
{
    namespace Logger
    {
        class Interface
        {
        public:
            virtual void Debug(const std::string &) = 0;
            virtual void Info(const std::string &) = 0;
            virtual void Warning(const std::string &) = 0;
            virtual void Error(const std::string &) = 0;
            virtual void Fatal(const std::string &) = 0;
        };

    }; //LOGGER
}; //SGX


#endif // SGX_LOGGER_INTERACE_H_