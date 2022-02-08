#ifndef LOGS_EVENT_EMITTER_INTERFACE_H_
# define LOGS_EVENT_EMITTER_INTERFACE_H_

class LogsEventEmitterInterface
{

public:

    virtual      ~LogsEventEmitterInterface() {};
    virtual void Info(const QString &) = 0;
    virtual void Success(const QString &) = 0;
    virtual void Warning(const QString &) = 0;
    virtual void Error(const QString &) = 0;

};

#endif // LOGS_EVENT_EMITTER_INTERFACE_H_