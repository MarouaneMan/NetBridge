#ifndef LOGS_CONSOLE_DIALOG_H_
# define LOGS_CONSOLE_DIALOG_H_

#include <QtWidgets/qdialog.h>
#include "ui_logs_console.h"
#include "logs_console_event_emitter.h"

class LogsConsoleDialog : public QDialog
{
    Q_OBJECT

public:

    LogsConsoleDialog(QWidget *parent = 0);
    ~LogsConsoleDialog();

    LogsConsoleEventEmitter	&GetEventEmitter();

    public slots:

    void		Info(const QString &);
    void		Success(const QString &);
    void		Warning(const QString &);
    void		Error(const QString &);
    void		ClearLogs();

private:

    void					logInternal(const QString &, const QString &);
    void					connectSignalsNSlots();

    Ui::LogsConsoleDialog	ui;
    LogsConsoleEventEmitter	eventEmitter;
};

#endif //LOGS_CONSOLE_DIALOG_H_