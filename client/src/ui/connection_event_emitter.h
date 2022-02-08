#ifndef CONNECTION_EVENT_EMITTER_H_
# define CONNECTION_EVENT_EMITTER_H_

#include <qobject.h>
#include "../connection_event_emitter_interface.h"

class ConnectionEventEmitter : public QObject, public ConnectionEventEmitterInterface
{
    Q_OBJECT;

public:

    ConnectionEventEmitter(QObject *parent = 0);
    ~ConnectionEventEmitter();

    void		AddConnection(Connection::ConnInfo);
    void		DeleteConnection(Connection::ConnInfo);

signals:

    void		addConnectionEvent(Connection::ConnInfo);
    void		deleteConnectionEvent(Connection::ConnInfo);

};

#endif //CONNECTION_EVENT_EMITTER_H_