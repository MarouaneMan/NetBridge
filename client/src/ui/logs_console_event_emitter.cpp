#include "logs_console_event_emitter.h"

LogsConsoleEventEmitter::LogsConsoleEventEmitter(QObject *parent) : QObject(parent)
{
}

LogsConsoleEventEmitter::~LogsConsoleEventEmitter()
{
}

void		LogsConsoleEventEmitter::Info(const QString &msg)
{
    emit infoEvent(msg);
}

void		LogsConsoleEventEmitter::Success(const QString &msg)
{
    emit successEvent(msg);
}

void		LogsConsoleEventEmitter::Warning(const QString &msg)
{
    emit warningEvent(msg);
}

void		LogsConsoleEventEmitter::Error(const QString &msg)
{
    emit errorEvent(msg);
}