#include "logs_console_dialog.h"
#include <qdatetime.h>
#include <qdebug.h>

LogsConsoleDialog::LogsConsoleDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    ui.log_text_edit->setFontPointSize(32);
    this->connectSignalsNSlots();
}

LogsConsoleDialog::~LogsConsoleDialog()
{
}

LogsConsoleEventEmitter	&LogsConsoleDialog::GetEventEmitter()
{
    return eventEmitter;
}

void		LogsConsoleDialog::Info(const QString &msg)
{
    logInternal(msg, "#000000");
}

void		LogsConsoleDialog::Success(const QString &msg)
{
    logInternal(msg, "#1e8c2e");
}

void		LogsConsoleDialog::Warning(const QString &msg)
{
    logInternal(msg, "#e58500");
}

void		LogsConsoleDialog::Error(const QString &msg)
{
    logInternal(msg, "#ff0000");
}

void		LogsConsoleDialog::ClearLogs()
{
    ui.log_text_edit->clear();
}

void		LogsConsoleDialog::logInternal(const QString &msg, const QString &color)
{

    QTextCursor cursor = ui.log_text_edit->textCursor(); // retrieve the visible cursor
    cursor.movePosition(QTextCursor::End); // move/modify/etc.
    ui.log_text_edit->setTextCursor(cursor); // apply visible cursor


    ui.log_text_edit->textCursor().insertHtml(
        QString(R"(<span style="color:%1;">[%2]&emsp;%3</span>)").
        arg(color).
        arg(QDateTime::currentDateTime().toString("dd-MM hh:mm:ss")).
        arg(msg)
    );
    ui.log_text_edit->textCursor().insertBlock();
}

void		LogsConsoleDialog::connectSignalsNSlots()
{

    // Info Log Event
    connect(&eventEmitter,					// Emitter
        SIGNAL(infoEvent(const QString &)),	// Signal
        this,								// Target
        SLOT(Info(const QString &)));		// Slot

    // Success Log Event
    connect(&eventEmitter,
        SIGNAL(successEvent(const QString &)),
        this,
        SLOT(Success(const QString &)));

    // Warning Log Event
    connect(&eventEmitter,
        SIGNAL(warningEvent(const QString &)),
        this,
        SLOT(Warning(const QString &)));

    // Error Log Event
    connect(&eventEmitter,
        SIGNAL(errorEvent(const QString &)),
        this,
        SLOT(Error(const QString &)));

    // Clear Logs Event
    connect(ui.clear_logs_button,
        SIGNAL(clicked()),
        this,
        SLOT(ClearLogs()));
}