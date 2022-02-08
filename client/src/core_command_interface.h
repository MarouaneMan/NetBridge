#ifndef CORE_COMMAND_INTERFACE_H_
# define CORE_COMMAND_INTERFACE_H_

#include <qstring.h>

class CoreCommandInterface
{

public:
    virtual         ~CoreCommandInterface() {};
    virtual void	AddHost(const QString &) = 0;
    virtual void	DeleteHost(const QString &) = 0;

};

#endif 