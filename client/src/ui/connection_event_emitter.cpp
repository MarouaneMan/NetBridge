#include "connection_event_emitter.h"

ConnectionEventEmitter::ConnectionEventEmitter(QObject *parent) : QObject(parent)
{
}

ConnectionEventEmitter::~ConnectionEventEmitter()
{
}

void		ConnectionEventEmitter::AddConnection(Connection::ConnInfo conn)
{
    emit addConnectionEvent(conn);
}

void		ConnectionEventEmitter::DeleteConnection(Connection::ConnInfo conn)
{
    emit deleteConnectionEvent(conn);
}
