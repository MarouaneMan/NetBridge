#ifndef STORE_H_
# define STORE_H_

#include "store_interface.h"

namespace Store
{
    class HostsStore : public HostsStoreInterface
    {

    public:

        HostsStore();
        ~HostsStore();

        const QStringList	&LoadHosts();
        void				AddHost(const QString &);
        void				DeleteHost(const QString &);

    private:

        void				persist();

        QStringList			hosts;
        QString				filePath;
    };
}

#endif // STORE_H_