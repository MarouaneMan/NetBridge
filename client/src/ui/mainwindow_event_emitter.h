#ifndef MAINWINDOW_EVENT_EMITTER_H_
# define MAINWINDOW_EVENT_EMITTER_H_

#include <qobject.h>
#include "../main_event_emitter_interface.h"

class MainWindowEventEmitter : public QObject, public MainEventEmitterInterface
{
    Q_OBJECT;

public:

    MainWindowEventEmitter(QObject *parent = 0);
    ~MainWindowEventEmitter();

    void SetHostID(const QString &);
    void SetBridgeAddress(const QString &);
    void SetBridgeState(int);
    void AddHost(const Host *);
    void UpdateHost(const Host *);
    void UpdateIncomingConnectionsCounter(int);
    void UpdateOutgoingConnectoinsCounter(int);

signals:

    void setHostIDEvent(const QString &);
    void setBridgeAddressEvent(const QString &);
    void setBridgeStateEvent(int);
    void addHostEvent(const Host *);
    void updateHostEvent(const Host *);
    void updateIncomingConnectionsCounterEvent(int);
    void updateOutgoingConnectionsCounterEvent(int);

};

#endif //MAINWINDOW_EVENT_EMITTER_H_