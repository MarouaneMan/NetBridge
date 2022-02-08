#include "config.h"
#include <qdir.h>
#include <qdebug.h>

Config			*Config::self_instance = nullptr;

Config::Config()
{
}

Config			*Config::Instance()
{
    if (self_instance == nullptr)
        self_instance = new Config();
    return self_instance;
}

Config			*Config::SetIniPath(const QString &ini_dir_path, const QString &ini_filename)
{
    this->ini_path = QDir(ini_dir_path).filePath(ini_filename);
    qDebug() << "config_file_path : " << this->ini_path;
    return this;
}

Config			*Config::Build()
{
    settings = std::make_unique<QSettings>(ini_path, QSettings::IniFormat);
    return this;
}

QVariant		Config::Value(const QString &key, const QVariant &default_value) const
{
    return settings->value(key, default_value);
}