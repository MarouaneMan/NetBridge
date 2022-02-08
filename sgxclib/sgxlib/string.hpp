#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <string>
#include <boost/algorithm/string.hpp>
#include <vector>

namespace SGX
{
    namespace String
    {
        // Null terminates std::array before copy to std::string
        template<class Type, size_t Size>
        inline void SafeCopyStdArrayToString(std::string &dest, std::array<Type, Size> src)
        {
            src[Size - 1] = 0;
            dest = static_cast<Type *>(&src[0]);
        }

        // Null terminates C Style array src before copy to std::string
        template<typename T, int size>
        inline void SafeCopyCArrayToString(std::string &dest, T(&src)[size])
        {
            src[size - 1] = 0;
            dest = src;
        }

        // Split string into vector tokens
        inline std::vector<std::string> Split(const std::string &src, const std::string &delimiter)
        {
            std::vector<std::string> v;
            boost::split(v, src, boost::is_any_of(delimiter));
            return v;
        }

        // Null terminates std::array before copy to std::array
        template<class Type, size_t Size>
        inline void SafeCopyStdArrayToStdArray(std::array<Type, Size> &dst, std::array<Type, Size> &src)
        {
            src[Size - 1] = 0;
            std::memcpy(&dst[0], &src[0], Size);
        }

        // Null terminates std::array
        template<class Type, size_t Size>
        inline void NullTerminateStdArray(std::array<Type, Size> &src)
        {
            src[Size - 1] = 0;
        }

    }; // STRING
}; // SGX

#endif //STRING_UTILS_H_