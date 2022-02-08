#include "ip.h"
#include <sstream>
#include <boost/asio.hpp>

// String vector x.x.x.x to uint32_t ip
unsigned int SGX::Net::IP::VectorToIPV4(const std::vector<std::string> &vec)
{
    auto toInt = [&](size_t i) -> int { return std::atoi(vec[i].c_str()); };
    return (toInt(0) << 24) | (toInt(1) << 16) | (toInt(2) << 8) | toInt(3);
}

// Get Next IP from range (from_address,to_address)
std::string SGX::Net::IP::GetNextIP(const std::string &from_address, const std::string &to_address)
{
    unsigned int begin = IP::VectorToIPV4(String::Split(from_address, "."));
    unsigned int end = IP::VectorToIPV4(String::Split(to_address, "."));
    for (unsigned int i = begin + 1; i <= end; i++)
    {
        uint8_t b4 = (i >> 24) & 0xff;
        uint8_t b3 = (i >> 16) & 0xff;
        uint8_t b2 = (i >> 8) & 0xff;
        uint8_t b1 = i & 0xff;
        if (b1 == 0 || b1 == 255)
            continue;
        std::stringstream ss;
        ss << (int)b4 << "." << (int)b3 << "." << (int)b2 << "." << (int)b1;
        return ss.str();
    }
    return from_address;
}

// Get machine Hostname
std::string SGX::Net::IP::GetHostname()
{
    return boost::asio::ip::host_name();
}

// Host to Network short
unsigned short SGX::Net::IP::htons(unsigned short a) { return ::htons(a); }

// Network to Host short
unsigned short SGX::Net::IP::ntohs(unsigned short a) { return ::ntohs(a); }

// Host to Network long
unsigned long SGX::Net::IP::htonl(unsigned long a) { return ::htonl(a); }

// Network to Host long
unsigned long SGX::Net::IP::ntohl(unsigned long a) { return ::ntohl(a); }