#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "Transport.h"

namespace ORU
{
/**
 * \brief Client class
 */
class ClientService
{

  private:
    // File descriptor for the client socket
    int _clientFd;
    // Structure to hold the address information for Unix domain socket communication
    struct sockaddr_un _address;
    // Length of the address structure
    int _addrlen;
    // Socket path
    std::string _socketPath { "/tmp/unix.socket" };

    void processMessage(); ///< Process incomming message

  public:
    ClientService();  ///< Constructor
    ~ClientService(); ///< Destructor

    int run(MessageType msg);            ///< Function to start service
    int quit();                          ///< Function to stop service
    int createCommunicationServices();   ///< Create transport
    void destroyCommunicationServices(); ///< Destroy transport

}; // class ClientService

} // namespace ORU

