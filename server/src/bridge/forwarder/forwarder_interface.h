#ifndef FORWARDER_INTERFACE_H_
# define FORWARDER_INTERFACE_H_

#include <string>

namespace Forwarder
{
  namespace TCP
  {
    
    class Interface
    {

    public:

        // Return random port on success, 0 on failure
        virtual unsigned short  Init() = 0;
        virtual void            Run(const std::string &hostID, const std::string &targetHostID, unsigned short remotePort, unsigned long readSomeSize) = 0;

    };
  };

  namespace UDP
  {
      class Interface
      {

      public:

          // Return random udp port on success, 0 on failure
          virtual unsigned short    Init() = 0;
          virtual void              Run() = 0;
      };
  };

};

#endif //FORWARDER_TCP_H_
