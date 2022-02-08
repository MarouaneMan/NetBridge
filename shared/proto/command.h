#ifndef COMMAND_H_
# define COMMAND_H_

#include <array>

namespace Command
{
    namespace AttrType
    {
        using Hostname = std::array<char, 64>;
        using HostID = std::array<char, 32>;
        using LocalIP = std::array<char, 52>;
        using Short = unsigned short;
        using Port = unsigned short;
        using Proto = enum Protocol { TCP, UDP };
        using UInt32 = uint32_t;
        using UInt8 = uint8_t;
    };

    namespace Request
    {
        enum Type
        {
            REGISTER,
            DISCOVER_HOST,
            DELETE_RELATIONSHIP,
            FORWARD,
            UDP_PACKET_DESC,
        };

        struct BaseRequest
        {
            Command::Request::Type  Type;
            AttrType::HostID	    HostID;
        };

        struct Register : BaseRequest
        {
            AttrType::Hostname	Hostname;
            AttrType::LocalIP	LocalIP;
            AttrType::UInt32    ReadSomeSize;
        };

        struct DiscoverHost : BaseRequest 
        {
            AttrType::UInt8   UDPEnabled;
        };

        struct DeleteRelationship : BaseRequest {};

        struct Forward : BaseRequest
        {
            AttrType::Short     ForwardID;
            AttrType::Proto     Protocol;
            AttrType::Port      RemotePort;
        };

        struct UDPPacketDesc
        {
            AttrType::UInt32    PacketSize;
            AttrType::Port      Port;
        };
    };


    namespace Response
    {
        enum Type
        {
            HOST_FOUND,
            HOST_NOTFOUND,
            CURRENT_HOST_DISCOVERED,
            HOST_DIED,
			HOST_UP,
            RELATIONSHIP_DELETED,
            FORWARD_ACCEPTED,
            FORWARD_ORDER,
            FORWARD_ERROR,
            UDP_FORWARDER_ENDPOINT,
        };

        struct BaseResponse
        {
            Command::Response::Type Type;
            AttrType::HostID        HostID;
        };

        struct HostResp : BaseResponse
        {
            AttrType::Hostname	Hostname;
            AttrType::LocalIP	LocalIP;
        };

        struct HostFound : HostResp {};

        struct HostNotFound : BaseResponse {};

        struct CurrentHostDiscovered : HostResp {};

        struct HostDied : BaseResponse {};

        struct HostUp : BaseResponse {};

        struct DeleteHost : BaseResponse {};

        struct RelationshipDeleted : BaseResponse {};

        struct ForwardAccepted : BaseResponse
        {
            AttrType::Proto     Protocol;
            AttrType::Short     ForwardID;
            AttrType::Port      RemotePort;
            AttrType::Port      BridgeRandomPort;
        };

        struct ForwardOrder : BaseResponse
        {
            AttrType::Proto     Protocol;
            AttrType::Hostname  FromHostname;
            AttrType::LocalIP   FromLocalIP;
            AttrType::Port      LocalPort;
            AttrType::Port      BridgeRandomPort;
            AttrType::UInt32    ReadSomeSize;
        };

        struct ForwardError : BaseResponse 
        { 
            AttrType::Proto     Protocol;
            AttrType::Short     ForwardID;
            AttrType::Port      RemotePort;
        };

        struct UDPPacketDesc
        {
            AttrType::UInt32    PacketSize;
            AttrType::Port      Port;
        };

        struct UDPForwarderEndPoint : BaseResponse
        {
            AttrType::Port      Port;
        };
    }
};



# endif // COMMAND_H_
