#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace ORU
{
/**
 * \brief Server class
 */
class ServerService
{

  private:
    // File descriptor for the server socket
    int _serverFd;
    // File descriptor for the client socket
    int _clientFd;
    // Structure to hold the address information for Unix domain socket communication
    struct sockaddr_un _address;
    // Length of the address structure
    int _addrlen;
    // Socket path
    std::string _socketPath { "/tmp/unix.socket" };
    // PID file
    std::string _filename { "/tmp/server.pid" };
    // Loop status
    bool _loop { true };

    void processMessage(); ///< Process incomming request
    void setPidFile();     ///< Create PID file
    int getPidFile();      ///< Read PID file

  public:
    ServerService();  ///< Constructor
    ~ServerService(); ///< Destructor

    int run();                           ///< Function to start service
    int quit();                          ///< Function to stop service
    void core();                         ///< Core function
    int createCommunicationServices();   ///< Create transport
    void destroyCommunicationServices(); ///< Destroy transport

}; // class ServerService

} // namespace ORU

