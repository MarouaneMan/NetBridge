#include "machineid.h"
#include <qstandardpaths.h>
#include <qdir.h>
#include <qdebug.h>
#include <random>
#include <cmath>
#include "sgxlib/rand.hpp"
#include "../common.h"

QString MachineID::machineid;

const QString &MachineID::Get()
{
    if (!machineid.isEmpty())
        return machineid;

    // Locate machine id file
    QString filePath = QDir(
        QStandardPaths::writableLocation(QStandardPaths::TempLocation)
    ).filePath(MACHINE_ID_FILENAME);
    qDebug() << "machine_id_file_path : " << filePath;

    // Check if id already exists
    QFile file(filePath);
    QString machine_id_str;
    if (file.open(QIODevice::ReadWrite))
    {
        // Read it
        QTextStream in(&file);
        machine_id_str = in.readAll();
    }

    // If empty generate new one
    if (machine_id_str.isEmpty())
    {
        // Rand generator
        SGX::Rand::Bounded<uint16_t> rd(100,999);

        machine_id_str = QString("%1 %2 %3")
            .arg(rd.Next(), 3, 10, QChar('0'))
            .arg(rd.Next(), 3, 10, QChar('0'))
            .arg(rd.Next(), 3, 10, QChar('0'));
        // Save it
        QTextStream out(&file);
        out << machine_id_str;
    };

    machineid = machine_id_str;

    return machineid;
}