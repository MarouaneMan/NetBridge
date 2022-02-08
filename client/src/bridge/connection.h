#ifndef CONNECTION_H_
# define CONNECTION_H_

#include "sgxlib/net/socket_interface.h"
#include <qobject.h>

namespace Connection
{
    enum Proto { TCP, UDP };
    enum Direction { INCOMING, OUTGOING };

    static const char *ProtoType[] = { "TCP", "UDP" };
    static const char *Dir[] = { "INCOMING", "OUTGOING" };

    struct ConnInfo
    {

        QString                             HostID;
        QString                             Hostname;
        QString                             IPAddress;
        Proto		                        Proto;
        uint16_t	                        BridgePort;
        uint16_t                            TargetPort;
        
        QString GetProto() { return ProtoType[Proto]; }

    };

    struct SockInfo
    {
        using Ptr = std::unique_ptr<SockInfo>;

        Direction                           Direction;
        ConnInfo                            ConnInfo;
        unsigned long                       ReadSomeSize;

        QString GetDirection() 
        { 
            return Dir[Direction]; 
        }

        QString ToString()
        {
            return 
                QString("HostID = %1, TargetPort = %2, BridgePort = %3, Proto = %4, Direction = %5")
                .arg(ConnInfo.HostID)
                .arg(ConnInfo.TargetPort)
                .arg(ConnInfo.BridgePort)
                .arg(ConnInfo.GetProto())
                .arg(GetDirection());
        }
    };
};

Q_DECLARE_METATYPE(Connection::ConnInfo);

#endif //CONNECTION_H_