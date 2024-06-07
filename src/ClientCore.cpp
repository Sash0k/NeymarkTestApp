#include <cstdlib>
#include <string>

#include "ClientService.h"
#include "Transport.h"

using namespace std;

namespace ORU
{
/**
 * \brief Client service class
 */

/**
 * @brief Client service constructor
 */
ClientService::ClientService()
	: _addrlen(sizeof(_address))
{
}

/**
 * @brief Client service destructor
 */
ClientService::~ClientService()
{
}

/**
 * @brief Creates communication services for the ClientService.
 *
 * This function sets up a Unix domain socket for communication with clients.
 * It creates a socket, binds it to a specific address, and starts listening for incoming connections.
 *
 * @return 0 if the communication services are successfully created,
 *         -1 otherwise.
 */
int ClientService::createCommunicationServices()
{
    _address.sun_family = AF_UNIX;
    strncpy(_address.sun_path, _socketPath.c_str(), sizeof(_address.sun_path) - 1);
    if ((_clientFd = socket(AF_UNIX, SOCK_STREAM, 0/*protocol*/)) == -1)
    {
        cout << "Failed to create socket" << endl;
        return -1;
    }
    if (connect(_clientFd, (struct sockaddr*)&_address, _addrlen) == -1)
    {
        cout << "Failed to create connection to Server" << endl;
        return -1;
    }
    return 0;
}

/**
 * @brief Destroys communication services for the ClientService.
 *
 * This function closes the server socket and removes the Unix domain socket file..
 */
void ClientService::destroyCommunicationServices()
{
    close(_clientFd);
}

/**
 * @brief Processing incomming message.
 *
 * This function parse incomming message and process it
 */
void ClientService::processMessage()
{
    char buffer[128];
    int numBytes = read(_clientFd, buffer, sizeof(buffer));
    cout << "Received " << numBytes << " bytes" << endl;

    if (numBytes > 0)
    {
        Header* header = reinterpret_cast<Header*>(buffer);
        Status* status = reinterpret_cast<Status*>(buffer);
        cout << "Receive message:" << endl;
        cout << "Version: " << static_cast<int>(header->version) << endl;
        cout << "Seq number: " << static_cast<int>(header->seqNum) << endl;
        cout << "Message type: " << static_cast<int>(header->type) << endl;
        cout << "Message size: " << static_cast<int>(header->size) << endl;
        cout << "Message status: " << static_cast<int>(header->status) << endl;

        switch (header->type)
        {
        case MSG_PONG:
            cout << "Receive PONG message" << endl;
            break;
        case MSG_RESP_STATUS:
            cout << "Receive RESP_STATUS message: " << static_cast<int>(status->status) << endl;
            break;
        case MSG_ERROR:
            cout << "Receive ERROR message" << endl;
            break;
        default:
            cout << "Receive unsupported message" << endl;
            break;
        }
    }
}

/**
 * @brief Core function of the ClientService.
 *
 * This function sends a client connection,
 * reads a message from the client, output it, sends a response back to the client,
 * and then closes the connection.
 *
 * @return 0 if the operation is successful, -1 otherwise.
 */
int ClientService::run(MessageType msg)
{
    cout << "Client was started" << endl;

    int res = createCommunicationServices();
    if (res)
    {
        cout << "Failed to create communication service" << endl;
        return -1;
    }

    cout << "Communication service was successfully created" << endl;

    if (msg == MSG_PING)
    {
        Ping req = Transport::createPingMsg();
        if (send(_clientFd, &req/*message*/, sizeof(req)/*length*/, 0/*flags*/) == -1)
        {
            cout << "Failed to send message to Server" << endl;
            return -1;
        }
    }
    else
    {
        Status req = Transport::createReqStatusMsg();
        if (send(_clientFd, &req/*message*/, sizeof(req)/*length*/, 0/*flags*/) == -1)
        {
            cout << "Failed to send message to Server" << endl;
            return -1;
        }
    }
    processMessage();

    destroyCommunicationServices();
    cout << "Client was stoped" << endl;

    return 0;
}

/**
 * @brief Stop ClientService.
 *
 * @return 0 if the operation is successful, -1 otherwise.
 */
int ClientService::quit()
{
    cout << "This command is not supported" << endl;
    return 0;
}

} // namespace ORU
