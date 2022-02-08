#include "store.h"
#include <qstandardpaths.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qregularexpression.h>
#include <qdebug.h>
#include "../common.h"

Store::HostsStore::HostsStore()
{
    // Hosts file
    filePath = QDir(
        QStandardPaths::writableLocation(QStandardPaths::TempLocation)
    ).filePath(HOSTS_FILENAME);

    qDebug() << "hosts_file_path" << filePath;
}

Store::HostsStore::~HostsStore()
{
}

const QStringList	&Store::HostsStore::LoadHosts()
{
    // Read & parse it
    QFile file(filePath);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream in(&file);
        QString hostsText = in.readAll();
        hosts = hostsText.split(QRegularExpression("\\r?\\n"), QString::SkipEmptyParts);
    }
    return hosts;
}

void		Store::HostsStore::AddHost(const QString &host)
{
    hosts.append(host);
    persist();
}

void		Store::HostsStore::DeleteHost(const QString &host)
{
    hosts.removeOne(host);
    persist();
}

void		Store::HostsStore::persist()
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&file);
        QString text = hosts.join("\r\n");
        out << text;
    }
}