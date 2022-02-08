#ifndef SGX_SIMPLE_LOGGER_H_
# define SGX_SIMPLE_LOGGER_H_

#include "logger.hpp"
#include <mutex>
#include <iostream>
#include "../date.hpp"

namespace SGX
{
    namespace Logger
    {
        class SimpleLogger : public Logger::Interface
        {

        public:

            void Debug(const std::string &msg)
            {
                logInternal("DEBUG", msg);
            }

            void Info(const std::string &msg)
            {
                logInternal("INFO ", msg);
            }

            void Warning(const std::string &msg)
            {
                logInternal("WARN ", msg);
            }

            void Error(const std::string &msg)
            {
                logInternal("ERROR", msg);
            }

            void Fatal(const std::string &msg)
            {
                logInternal("FATAL", msg);
            }

        private:

            void logInternal(const std::string &level, const std::string &msg)
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::cout << "[" << SGX::Date::getDateTime("%d-%m %H:%M:%S") << "]" << " [" << level << "] " << msg << std::endl;
            }

            std::mutex mutex;
        };

    }; //LOGGER
}; //SGX

#endif // SGX_SIMPLE_LOGGER_H_