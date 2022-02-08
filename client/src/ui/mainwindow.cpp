#include <qtablewidget.h>
#include "mainwindow.h"
#include <qdebug.h>
#include <qmessagebox.h>
#include "../bridge/bridge_client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    incomingConnectionsDialog(this),
    incomingConnectionsConnection(0),
    outgoingConnectionsDialog(this),
    outgoingConnectionsCounter(0),
    logsConsoleDialog(this),
    trayIcon(this)
{
    ui.setupUi(this);

    setWindowTitle(PROJECT_TITLE);
    
    ui.hosts_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.hosts_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui.add_host_input->setValidator(new QRegExpValidator(Host::HostRegexp));

    // TrayIcon
    trayIcon.setIcon(QIcon(":/icon.ico"));
    createTrayContextMenu();
    trayIcon.setVisible(true);
    trayIcon.show();

    // Connect signals
    connectSignalsNSlots();
}

MainWindow::~MainWindow()
{
}

void	MainWindow::SetHostID(const QString &hostID)
{
    ui.host_id_input->setText(hostID);
}

void	MainWindow::SetBridgeAddress(const QString &bridgeAddress)
{
    ui.bridge_input->setText(bridgeAddress);
}

void	MainWindow::SetBridgeState(int state)
{
    QColor color = Qt::white;

    if (state == BridgeClient::State::Connected)
    {
        // Enable add host
        ui.add_host_input->setEnabled(true);
        ui.add_host_button->setEnabled(true);

        color = COLOR_GREEN;
    }
    else if (state == BridgeClient::State::Disconnected)
    {
        // Disable add host
        ui.add_host_input->setDisabled(true);
        ui.add_host_button->setDisabled(true);

        // Set all hosts to down
        for (int i = 0; i < ui.hosts_table->rowCount(); i++)
        {
            // Update background cells color
            for (int j = 0; j < 4; j++)
                ui.hosts_table->item(i, j)->setBackground(COLOR_RED);
        }
        color = COLOR_RED;
    }
    QPalette palette = ui.bridge_input->palette();
    palette.setColor(QPalette::Base, color);
    ui.bridge_input->setPalette(palette);
}

void	MainWindow::AddHost(const Host *host)
{
    ui.hosts_table->insertRow(ui.hosts_table->rowCount());
    int i = ui.hosts_table->rowCount() - 1;
    ui.hosts_table->setItem(i, 0, new QTableWidgetItem(host->ID));
    ui.hosts_table->setItem(i, 1, new QTableWidgetItem(host->LoopBack));
    ui.hosts_table->setItem(i, 2, new QTableWidgetItem(host->HostName));
    ui.hosts_table->setItem(i, 3, new QTableWidgetItem(host->IpAddress));

    // Add button with hostID as object name
    QPushButton *btn = new QPushButton("Delete");
    btn->setObjectName(host->ID);
    connect(btn, SIGNAL(clicked()), this, SLOT(OnDeleteHostClicked()));
    ui.hosts_table->setCellWidget(i, 4, btn);
}

void	MainWindow::UpdateHost(const Host *host)
{
    for (int i = 0; i < ui.hosts_table->rowCount(); i++)
    {
        if (ui.hosts_table->item(i, 0)->text() == host->ID)
        {
            // Pick color
            QColor color;
            if (host->State == Host::Down)
                color = COLOR_RED;
            else if (host->State == Host::Up)
                color = COLOR_GREEN;
            else if (host->State == Host::Error)
                color = COLOR_ORANGE;

            // Update Hostname
            ui.hosts_table->setItem(i, 2, new QTableWidgetItem(host->HostName));

            // Update IPAddress
            ui.hosts_table->setItem(i, 3, new QTableWidgetItem(host->IpAddress));

            // Update background cells color
            for (int j = 0; j < 4; j++)
                ui.hosts_table->item(i, j)->setBackground(color);
            break;
        }
    }
}

void	MainWindow::UpdateIncomingConnectionsCounter(int c)
{
    incomingConnectionsConnection += c;
    ui.incoming_connections_button->setText(QString("Incoming connections (%1)").arg(incomingConnectionsConnection));
}

void	MainWindow::UpdateOutgoingConnectionsCounter(int c)
{
    outgoingConnectionsCounter += c;
    ui.outgoing_connections_button->setText(QString("Outgoing connections (%1)").arg(outgoingConnectionsCounter));
}

void	MainWindow::OnIncomingConnectionsClicked()
{
    incomingConnectionsDialog.show();
}

void	MainWindow::OnOutgoingConnectionsClicked()
{
    outgoingConnectionsDialog.show();
}

void	MainWindow::OnLogsClicked()
{
    logsConsoleDialog.show();
}

void	MainWindow::OnAddHostClicked()
{
    QString newHostID = Host::ParseHostID(ui.add_host_input->text());
    if (newHostID.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Invalid Host ID");
    }
    else
    {
        coreCommandInterface->AddHost(newHostID);
        ui.add_host_input->clear();
    }
}

void	MainWindow::OnDeleteHostClicked()
{
    QPushButton *btn = dynamic_cast<QPushButton*>(sender());
    const QString &hostID = btn->objectName();
    for (int i = 0; i < ui.hosts_table->rowCount(); i++)
    {
        if (ui.hosts_table->item(i, 0)->text() == hostID)
        {
            ui.hosts_table->removeRow(i);
            coreCommandInterface->DeleteHost(hostID);
            break;
        }
    }
}

void     MainWindow::OnIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // Show main window on double click
    switch (reason) 
    {
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    }
}

void    MainWindow::closeEvent(QCloseEvent *event)
{
    // Hide main window on close event
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
	incomingConnectionsDialog.hide();
	outgoingConnectionsDialog.hide();
	logsConsoleDialog.hide();
    if (trayIcon.isVisible()) {
        hide();
        event->ignore();
    }
}

void     MainWindow::createTrayContextMenu()
{
    // App can exit via Quit menu
    auto quitAction = new QAction("&Quit", this);
    
    // Connect Quit action to QCoreApplication::instance
    connect(quitAction,
        SIGNAL(triggered(bool)),
        this, 
        SLOT(OnQuit())
    );

    auto menu = new QMenu(this);
    menu->addAction(quitAction);
    trayIcon.setContextMenu(menu);
}

void	MainWindow::OnQuit()
{
	trayIcon.hide();
	exit(0);
}

void	MainWindow::connectSignalsNSlots()
{
    // HostID Event
    connect(&eventEmitter,							// Emitter
        SIGNAL(setHostIDEvent(const QString &)),	// Signal
        this,										// Target
        SLOT(SetHostID(const QString &)));			// Slot

    // BridgeAddress Event
    connect(&eventEmitter,
        SIGNAL(setBridgeAddressEvent(const QString &)),
        this,
        SLOT(SetBridgeAddress(const QString &)));

    // BridgeState Event
    connect(&eventEmitter,
        SIGNAL(setBridgeStateEvent(int)),
        this,
        SLOT(SetBridgeState(int)));

    // AddHost Event
    connect(&eventEmitter,
        SIGNAL(addHostEvent(const Host *)),
        this,
        SLOT(AddHost(const Host *)));

    // UpdatHost Event
    connect(&eventEmitter,
        SIGNAL(updateHostEvent(const Host *)),
        this,
        SLOT(UpdateHost(const Host *)));

    // Update IncomingConnections Counter Event
    connect(&eventEmitter,
        SIGNAL(updateIncomingConnectionsCounterEvent(int)),
        this,
        SLOT(UpdateIncomingConnectionsCounter(int)));

    // Update OutgoingConnections Counter Event
    connect(&eventEmitter,
        SIGNAL(updateOutgoingConnectionsCounterEvent(int)),
        this,
        SLOT(UpdateOutgoingConnectionsCounter(int)));

    // IncomingConnections Click Event
    connect(ui.incoming_connections_button,
        SIGNAL(clicked()),
        this,
        SLOT(OnIncomingConnectionsClicked()));

    // OutgoingConnections Click Event
    connect(ui.outgoing_connections_button,
        SIGNAL(clicked()),
        this,
        SLOT(OnOutgoingConnectionsClicked()));

    // Logs Click Event
    connect(ui.logs_button,
        SIGNAL(clicked()),
        this,
        SLOT(OnLogsClicked()));

    // AddHost Click Event
    connect(ui.add_host_button,
        SIGNAL(clicked()),
        this,
        SLOT(OnAddHostClicked()));

    // Tray Icon
    connect(&trayIcon,
        SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this,
        SLOT(OnIconActivated(QSystemTrayIcon::ActivationReason)));
}

MainEventEmitterInterface	&MainWindow::GetEventEmitter()
{
    return eventEmitter;
}

ConnectionEventEmitterInterface	&MainWindow::GetIncomingConnectionsEventEmitter()
{
    return incomingConnectionsDialog.GetEventEmitter();
}

ConnectionEventEmitterInterface	&MainWindow::GetOutgoingConnectionsEventEmitter()
{
    return outgoingConnectionsDialog.GetEventEmitter();
}

LogsEventEmitterInterface	&MainWindow::GetLogsConsoleEventEmitter()
{
    return logsConsoleDialog.GetEventEmitter();
}

void						MainWindow::SetCoreCommandInterface(CoreCommandInterface *coreCommandInterface)
{
    this->coreCommandInterface = coreCommandInterface;
}
