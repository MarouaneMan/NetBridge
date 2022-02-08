#include "abstract_command.h"

Command::Runner::Abstract::Abstract(
    SGX::Logger::Interface *logger, 
    HostsContainer *hostsContainer
) : 
    logger(logger), 
    hostsContainer(hostsContainer)
{
}

Command::Runner::Abstract::~Abstract()
{
}
