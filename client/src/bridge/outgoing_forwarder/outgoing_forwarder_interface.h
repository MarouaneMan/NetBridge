#ifndef OUTGOING_TCP_FORWARDER_INTERFACE_H_
# define OUTGOING_TCP_FORWARDER_INTERFACE_H_

#include <memory>

namespace Forwarder
{
    namespace Outgoing
    {
        namespace TCP
        {
            class Interface
            {

            public:

                using Ptr   = std::unique_ptr<TCP::Interface>;
                using SPtr  = std::shared_ptr<TCP::Interface>;

                virtual         ~Interface() {};
                virtual void    RunAcceptor() = 0;
                virtual void    Destroy() = 0;
                virtual void    StartForward(unsigned short, unsigned short) = 0;
                virtual void    KillForward(unsigned short) = 0;
            };
        };

        namespace UDP
        {
            class Interface
            {

            public:

                using Ptr = std::unique_ptr<UDP::Interface>;
                using SPtr = std::shared_ptr<UDP::Interface>;

                virtual         ~Interface() {};
                virtual void    Run() = 0;
                virtual void    Close() = 0;
            };
        }
    }



};

#endif //OUTGOING_TCP_FORWARDER_INTERFACE_H_
