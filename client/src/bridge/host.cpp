#include "host.h"

QRegExp			Host::HostRegexp = QRegExp(HOST_REGEX);

QString			Host::ParseHostID(const QString &hostID)
{
    int pos = HostRegexp.indexIn(hostID);
    if (pos > -1)
    {
        QStringList parsed = HostRegexp.capturedTexts();
        parsed.removeAt(0);
        return parsed.join(" ");
    }
    return "";
}
