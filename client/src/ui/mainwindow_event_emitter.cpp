#include "mainwindow_event_emitter.h"

MainWindowEventEmitter::MainWindowEventEmitter(QObject *parent) : QObject(parent)
{
}

MainWindowEventEmitter::~MainWindowEventEmitter()
{
}

void MainWindowEventEmitter::SetHostID(const QString &hostID)
{
    emit setHostIDEvent(hostID);
}

void MainWindowEventEmitter::SetBridgeAddress(const QString &bridgeAddress)
{
    emit setBridgeAddressEvent(bridgeAddress);
}

void MainWindowEventEmitter::SetBridgeState(int state)
{
    emit setBridgeStateEvent(state);
}

void MainWindowEventEmitter::AddHost(const Host *host)
{
    emit addHostEvent(host);
}

void MainWindowEventEmitter::UpdateHost(const Host *host)
{
    emit updateHostEvent(host);
}

void MainWindowEventEmitter::UpdateIncomingConnectionsCounter(int c)
{
    emit updateIncomingConnectionsCounterEvent(c);
}

void MainWindowEventEmitter::UpdateOutgoingConnectoinsCounter(int c)
{
    emit updateOutgoingConnectionsCounterEvent(c);
}
