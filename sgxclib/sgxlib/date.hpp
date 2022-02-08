#ifndef  SGX_DATE_H_
# define SGX_DATE_H_

#include <iomanip>
#include <boost/lexical_cast.hpp>

namespace SGX
{
    namespace Date
    {
        // getDateTime as formatted string :: "%d-%m %H:%M:%S"
        std::string getDateTime(const char *format)
        {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            return boost::lexical_cast<std::string>(std::put_time(&tm, format));
        }

    }; // DATE
}; // SGX

#endif // SGX_DATE_H_