#ifndef CONNECTION_EVENT_EMITTER_INTERFACE_H_
# define CONNECTION_EVENT_EMITTER_INTERFACE_H_

#include "bridge/connection.h"

class ConnectionEventEmitterInterface
{

public:
    virtual             ~ConnectionEventEmitterInterface() {};
    virtual void        AddConnection(Connection::ConnInfo) = 0;
    virtual void        DeleteConnection(Connection::ConnInfo) = 0;
};

#endif //CONNECTION_EVENT_EMITTER_INTERFACE_H_