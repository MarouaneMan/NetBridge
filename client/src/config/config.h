#ifndef CONFIG_H_
# define CONFIG_H_

#include <qstring.h>
#include <qsettings.h>
#include <memory>

class Config
{

public:

    static Config	*Instance();
    Config			*SetIniPath(const QString &ini_dir_path, const QString &ini_filename);
    Config			*Build();
    QVariant		Value(const QString &key, const QVariant &default_value) const;

private:

    Config();

    static Config				*self_instance;
    QString						ini_path;
    std::unique_ptr<QSettings>	settings;
};


// Config Helpers
#define CONFIG_VAL(key, default_value)	Config::Instance()->Value(key, default_value)
#define CONFIG_INT(key, default_value)	CONFIG_VAL(key, default_value).toInt()
#define CONFIG_STR(key, default_value)	CONFIG_VAL(key, default_value).toString()
#define CONFIG_BOOL(key, default_value) CONFIG_VAL(key, default_value).toBool()

#endif CONFIG_H_