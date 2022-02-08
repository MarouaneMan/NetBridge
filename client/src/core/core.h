#ifndef CORE_H_
# define CORE_H_

#include "../main_event_emitter_interface.h"
#include "../connection_event_emitter_interface.h"
#include "../logs_event_emitter_interface.h"
#include "../core_command_interface.h"
#include "../store/store_interface.h"
#include "../bridge/bridge_client.h"
#include "../bridge/hosts_container.h"

class Core : public CoreCommandInterface
{

public:

    Core(
        MainEventEmitterInterface &,
        ConnectionEventEmitterInterface &,
        ConnectionEventEmitterInterface &,
        Store::HostsStoreInterface &,
        LogsEventEmitterInterface &
    );
    ~Core();

    void			Run();
    void			AddHost(const QString &);
    void			DeleteHost(const QString &);

private:
    void			spawnBridgeClient();
    void			loadHosts();
    void			addHostInternal(const QString &);

    MainEventEmitterInterface				&mainEventEmitter;
    Store::HostsStoreInterface				&hostsStore;
    ConnectionEventEmitterInterface         &outgoingConnectionEventEmitter;
    LogsEventEmitterInterface				&logsEmitter;
    HostsContainer                          hostsContainer;
    QString									hostID;
    BridgeClient							bridgeClient;
};

#endif // CORE_H_