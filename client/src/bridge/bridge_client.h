#ifndef BRIDGE_CLIENT_H_
# define BRIDGE_CLIENT_H_

#include <qobject.h>
#include <memory>
#include "sgxlib/net/client_interface.h"
#include "sgxlib/callback_registrator.hpp"
#include "hosts_container.h"
#include "../main_event_emitter_interface.h"
#include "../connection_event_emitter_interface.h"
#include "../logs_event_emitter_interface.h"

class MainEventEmitterInterface;

class BridgeClient
{

public:

    enum State
    {
        Connected,
        Disconnected
    };

    BridgeClient(
        MainEventEmitterInterface &mainWindowEventEmitter,
        ConnectionEventEmitterInterface &incomingConnectionEventEmitter,
        ConnectionEventEmitterInterface	&outgoingConnectionEventEmitter,
        LogsEventEmitterInterface &logsEmitter,
        HostsContainer &hostsContainer
    );
    ~BridgeClient();

    void    Run(SGX::Net::Client::TCP::Interface::Ptr, std::string &);
    void    DiscoverHost(const QString &);
    void    DeleteHost(Host::Ptr);

private:
    
    void    registerHost();
    void    discoverHosts();
    void    hostFoundCallback();
    void    hostNotFoundCallBack();
    void    currentHostdiscoveredCallback();
    void    friendHostDiedCallback();
	void	friendHostUpCallback();
    void    relationshipDeletedCallback();
    void    forwardAcceptedCallback();
    void    forwardOrderCallback();
    void    forwardErrorCallback();
    void    udpForwardEndpoint();
    void    listenForResponses();

    MainEventEmitterInterface                           &mainEventEmitter;
    ConnectionEventEmitterInterface                     &incomingConnectionEventEmitter;
    ConnectionEventEmitterInterface                     &outgoingConnectionEventEmitter;
    LogsEventEmitterInterface                           &logsEmitter;
    SGX::Net::Socket::TCP::Interface::Ptr               commandServerSock;
    std::vector<char>                                   commandBuffer;
    std::string                                         hostID;
    HostsContainer                                      &hostsContainer;

    // CallbackRegistrator
    SGX::Callback::Registrator<
        int,            // KeyType
        void>           // ReturnType
        responseCallbackRegistrator;
};

#endif // BRIDGE_CLIENT_H_