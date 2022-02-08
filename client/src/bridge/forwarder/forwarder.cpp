#include "forwarder.h"
#include <thread>
#include <qdebug.h>

Forwarder::Forward::Forward(
    Connection::SockInfo::Ptr pairSock,
    Forwarder::Peer::Interface::Ptr src, 
    Forwarder::Peer::Interface::Ptr dst,
    LogsEventEmitterInterface &logsEmitter
)
: 
    sockInfo(std::move(pairSock)),
    src(std::move(src)), 
    dst(std::move(dst)),
    logsEmitter(logsEmitter)
{
}

void        Forwarder::Forward::Run()
{
    logsEmitter.Info(QString("Forwarder spawned : %1").arg(sockInfo->ToString()));

    // Init source connection
    bool src_initialized = src->Init();
    if (!src_initialized)
    {
        logsEmitter.Error(QString("Forwarder failed to init src connection : %1").arg(sockInfo->ToString()));
        return;
    }

    qDebug() << "Forwarder initialized src peer :" << sockInfo->ToString();

    // Init dest connection
    bool dst_initialiazed = dst->Init();
    if (!dst_initialiazed)
    {
        logsEmitter.Error(QString("Forwarder failed to init dst connection : %1").arg(sockInfo->ToString()));
        return;
    }

    qDebug() << "Forwarder initialized dst peer :" << sockInfo->ToString();

    // LogSuccess
    logsEmitter.Success(QString("Forwarder initialized (src,dst) peers : %1").arg(sockInfo->ToString()));

    // Forward packets
    {
        auto th = std::thread(&Forwarder::Forward::forward, this, src.get(), dst.get(), sockInfo->ReadSomeSize);
        forward(dst.get(), src.get(), sockInfo->ReadSomeSize);
        th.join();
    }

    // LogInfo
    logsEmitter.Info(QString("Forwarder terminated %1").arg(sockInfo->ToString()));
}

void       Forwarder::Forward::forward(Peer::Interface *src, Peer::Interface *dst, unsigned long readSomeSize)
{
    std::vector<char> buffer;
    buffer.resize(readSomeSize);

    while (true)
    {
        // Read from source
        size_t read_size = src->ReadSome(buffer);
        if (read_size == 0)
        {
            logsEmitter.Warning(QString("Source peer disconnected : %1").arg(sockInfo->ToString()));
            dst->Close();
            break;
        }

        // Write to dest
        bool sent = dst->WriteAll(&buffer[0], read_size);
        if (!sent)
        {
            logsEmitter.Warning(QString("Destination peer disconnected % 1").arg(sockInfo->ToString()));
            src->Close();
            break;
        }
    }
}

void		Forwarder::Forward::Close()
{
	src->Close();
	dst->Close();
}