#ifndef HOST_H_
# define HOST_H_

#include <qstring.h>
#include <qobject.h>
#include <qregexp.h>
#include <memory>
//#include "outgoing_forwarder/outgoing_tcp_forwarder_manager.h"
//#include "outgoing_forwarder/outgoing_udp_forwarder_manager.h"

namespace Forwarder {
    namespace Outgoing { 
        namespace TCP { class Manager; } 
        namespace UDP { class Manager; }
    }
}

struct Host
{
    using Ptr = std::shared_ptr<Host>;

    enum State {
        Down,
        Up,
        Error,
    };

    Host() : 
        State(Down), OutgoingTCPForwardersManager(nullptr), 
        OutgoingUDPForwardersManager(nullptr) {}
    
    QString                             ID;
    QString                             LoopBack;
    QString                             HostName;
    QString                             IpAddress;
    State                               State;
    Forwarder::Outgoing::TCP::Manager   *OutgoingTCPForwardersManager;
    Forwarder::Outgoing::UDP::Manager   *OutgoingUDPForwardersManager;
    static QRegExp                      HostRegexp;
    static QString                      ParseHostID(const QString &);
};

#define HOST_REGEX "^([0-9]{3})\\s?([0-9]{3})\\s?([0-9]{3})$"

Q_DECLARE_METATYPE(Host);

#endif // HOST_H_