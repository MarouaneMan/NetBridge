#ifndef LOGS_CONSOLE_EVENT_EMITTER_H_
# define LOGS_CONSOLE_EVENT_EMITTER_H_

#include <qobject.h>
#include "../logs_event_emitter_interface.h"

class LogsConsoleEventEmitter : public QObject, public LogsEventEmitterInterface
{
    Q_OBJECT;

public:

    LogsConsoleEventEmitter(QObject *parent = 0);
    ~LogsConsoleEventEmitter();

    void		Info(const QString &);
    void		Success(const QString &);
    void		Warning(const QString &);
    void		Error(const QString &);

signals:

    void		infoEvent(const QString &);
    void		successEvent(const QString &);
    void		warningEvent(const QString &);
    void		errorEvent(const QString &);
};

#endif //LOGS_CONSOLE_EVENT_EMITTER_H_