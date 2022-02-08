#include "outgoing_connections_dialog.h"

OutgoingConnectionsDialog::OutgoingConnectionsDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    this->connectSignalsNSlots();
    ui.table->horizontalHeader()->setStretchLastSection(true);
    ui.table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

OutgoingConnectionsDialog::~OutgoingConnectionsDialog()
{
}

ConnectionEventEmitter	&OutgoingConnectionsDialog::GetEventEmitter()
{
    return eventEmitter;
}

void		OutgoingConnectionsDialog::AddConnection(Connection::ConnInfo conn)
{
    ui.table->insertRow(ui.table->rowCount());
    int i = ui.table->rowCount() - 1;
    ui.table->setItem(i, 0, new QTableWidgetItem(conn.HostID));
    ui.table->setItem(i, 1, new QTableWidgetItem(conn.GetProto()));
    ui.table->setItem(i, 2, new QTableWidgetItem(QString::number(conn.TargetPort)));
    ui.table->setItem(i, 3, new QTableWidgetItem(QString::number(conn.BridgePort)));
    ui.table->setItem(i, 4, new QTableWidgetItem(conn.IPAddress));
    ui.table->setItem(i, 5, new QTableWidgetItem(conn.Hostname));
}

void		OutgoingConnectionsDialog::DeleteConnection(Connection::ConnInfo conn)
{
    for (int i = 0; i < ui.table->rowCount(); i++)
    {
        if (ui.table->item(i, 0)->text() == conn.HostID
            && ui.table->item(i, 3)->text() == QString::number(conn.BridgePort)) {
            ui.table->removeRow(i);
            break;
        }
    }
}

void		OutgoingConnectionsDialog::connectSignalsNSlots()
{
    // Add Incoming Connection Event
    connect(&eventEmitter,                                      // Emitter
        SIGNAL(addConnectionEvent(Connection::ConnInfo)),       // Signal
        this,                                                   // Target
        SLOT(AddConnection(Connection::ConnInfo)));             // Slot

    // Delete Incoming Connection Event
    connect(&eventEmitter,
        SIGNAL(deleteConnectionEvent(Connection::ConnInfo)),
        this,
        SLOT(DeleteConnection(Connection::ConnInfo)));
}