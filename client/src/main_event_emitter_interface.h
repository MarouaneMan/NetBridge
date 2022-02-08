#ifndef MAIN_EVENT_EMITTER_INTERFACE_H_
# define MAIN_EVENT_EMITTER_INTERFACE_H_

#include <qstring.h>

struct Host;

class MainEventEmitterInterface {

public:
    virtual      ~MainEventEmitterInterface() {};
    virtual void SetHostID(const QString &) = 0;
    virtual void SetBridgeAddress(const QString &) = 0;
    virtual void SetBridgeState(int) = 0;
    virtual void AddHost(const Host *) = 0;
    virtual void UpdateHost(const Host *) = 0;
    virtual void UpdateIncomingConnectionsCounter(int) = 0;
    virtual void UpdateOutgoingConnectoinsCounter(int) = 0;

};

#endif