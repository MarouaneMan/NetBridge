#ifndef STORE_INTERFACE_H_
# define STORE_INTERFACE_H_

#include <qstringlist.h>

namespace Store
{
    class HostsStoreInterface
    {

    public:

        virtual const QStringList	&LoadHosts() = 0;
        virtual void				AddHost(const QString &) = 0;
        virtual void				DeleteHost(const QString &) = 0;

    };

}


#endif // STORE_H_