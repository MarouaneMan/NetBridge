#ifndef OUTGOING_CONNECTIONS_WINDOW_H
# define OUTGOING_CONNECTIONS_WINDOW_H

#include <QtWidgets/qdialog.h>
#include "ui_outgoing_connections_dialog.h"
#include "connection_event_emitter.h"

class OutgoingConnectionsDialog : public QDialog
{
    Q_OBJECT

public:

    OutgoingConnectionsDialog(QWidget *parent = 0);
    ~OutgoingConnectionsDialog();

    ConnectionEventEmitter      	&GetEventEmitter();

    public slots:

    void							AddConnection(Connection::ConnInfo);
    void							DeleteConnection(Connection::ConnInfo);

private:

    void							connectSignalsNSlots();

    Ui::OutgoingConnectionDialog	ui;
    ConnectionEventEmitter      	eventEmitter;
};

#endif // OUTGOING_CONNECTIONS_WINDOW_H