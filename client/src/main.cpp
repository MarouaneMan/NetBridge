#include <QtWidgets/QApplication>
#include "config/config.h"
#include "ui/mainwindow.h"
#include "core/core.h"
#include "store/store.h"
#include <qdebug.h>

int		main(int argc, char *argv[])
{
    // UI
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    qRegisterMetaType<Connection::ConnInfo>();
    MainWindow w;
    w.show();

    // Config
    Config::Instance()
        ->SetIniPath(QCoreApplication::applicationDirPath(), CONFIG_FILENAME)
        ->Build();

    // HostsStore
    Store::HostsStore hostsStore;

    // Core
    Core core(
        w.GetEventEmitter(),                        // Main Event Emitter
        w.GetIncomingConnectionsEventEmitter(),     // Incoming Connection Event Emitter
        w.GetOutgoingConnectionsEventEmitter(),     // Outgoing Connection Event Emitter
        hostsStore,                                 // Hosts Store
        w.GetLogsConsoleEventEmitter()              // Logs Event Emitter
    );
    core.Run();
    w.SetCoreCommandInterface(&core);

    return a.exec();
}