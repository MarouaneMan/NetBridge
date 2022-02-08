#ifndef INCOMING_CONNECTIONS_WINDOW_H
# define INCOMING_CONNECTIONS_WINDOW_H

#include <QtWidgets/qdialog.h>
#include "ui_incoming_connections_dialog.h"
#include "connection_event_emitter.h"

class IncomingConnectionsDialog : public QDialog
{
    Q_OBJECT

public:

    IncomingConnectionsDialog(QWidget *parent = 0);
    ~IncomingConnectionsDialog();

    ConnectionEventEmitter	        &GetEventEmitter();

    public slots:

    void							AddConnection(Connection::ConnInfo);
    void							DeleteConnection(Connection::ConnInfo);

private:

    void							connectSignalsNSlots();

    Ui::IncomingConnectionDialog	ui;
    ConnectionEventEmitter	        eventEmitter;

};

#endif // INCOMING_CONNECTIONS_WINDOW_H