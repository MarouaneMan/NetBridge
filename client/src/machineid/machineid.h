#ifndef MACHINE_ID_
# define MACHINE_ID_

#include <qstring.h>

class MachineID
{

public:

    static const QString	&Get();

private:

    static QString			machineid;
};

#endif //MACHINE_ID_