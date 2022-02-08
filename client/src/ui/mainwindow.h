#ifndef MAINWINDOW_H_
# define MAINWINDOW_H_

#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include "../common.h"
#include "ui_mainwindow.h"
#include "mainwindow_event_emitter.h"
#include "../core_command_interface.h"
#include "incoming_connections_dialog.h"
#include "outgoing_connections_dialog.h"
#include "logs_console_dialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    MainEventEmitterInterface       &GetEventEmitter();
    ConnectionEventEmitterInterface &GetIncomingConnectionsEventEmitter();
    ConnectionEventEmitterInterface &GetOutgoingConnectionsEventEmitter();
    LogsEventEmitterInterface       &GetLogsConsoleEventEmitter();
    void                            SetCoreCommandInterface(CoreCommandInterface *);

    public slots:
    void                            SetHostID(const QString &);
    void                            SetBridgeAddress(const QString &);
    void                            SetBridgeState(int);
    void                            AddHost(const Host *);
    void                            UpdateHost(const Host *);
    void                            UpdateIncomingConnectionsCounter(int);
    void                            UpdateOutgoingConnectionsCounter(int);
    void                            OnIncomingConnectionsClicked();
    void                            OnOutgoingConnectionsClicked();
    void                            OnLogsClicked();
    void                            OnAddHostClicked();
    void                            OnDeleteHostClicked();
    void                            OnIconActivated(QSystemTrayIcon::ActivationReason);
	void							OnQuit();

protected:
    
    void                            closeEvent(QCloseEvent *event) override;

private:

    void                            connectSignalsNSlots();
    void                            createTrayContextMenu();

    Ui::MainWindow                  ui;
    QSystemTrayIcon                 trayIcon;
    MainWindowEventEmitter          eventEmitter;
    CoreCommandInterface            *coreCommandInterface;
    IncomingConnectionsDialog       incomingConnectionsDialog;
    OutgoingConnectionsDialog       outgoingConnectionsDialog;
    LogsConsoleDialog               logsConsoleDialog;
    int                             incomingConnectionsConnection;
    int                             outgoingConnectionsCounter;

};

#define COLOR_GREEN                 QColor(191, 255, 192)
#define COLOR_RED                   QColor(255, 191, 191)
#define COLOR_ORANGE                QColor(252, 178, 121)

#endif // MAINWINDOW_H_