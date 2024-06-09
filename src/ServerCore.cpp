#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <signal.h>

#include "Logger.h"
#include "ServerService.h"
#include "Transport.h"

using namespace std;

namespace ORU
{
/**
 * \brief Server service class
 */

/**
 * @brief Server service constructor
 */
ServerService::ServerService()
	: _addrlen(sizeof(_address))
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigHandler;
    sigset_t set; 
    sigemptyset(&set);                                                             
    sigaddset(&set, SIGINT); 
    act.sa_mask = set;
		
    sigaction(SIGINT, &act, 0);
}

/**
 * @brief Server service destructor
 */
ServerService::~ServerService()
{
}

/**
 * @brief Демонстрация обработки системных сигналов
 */
void ServerService::sigHandler(int sig)
{
	Logger::redirect cout_redir(std::cout);
	
    if(sig == SIGINT)
		cout << "ServerCore receive SIGINT!" << endl;
	else
		cout << "ServerCore receive signal: " + std::to_string(sig) << endl;
}

/**
 * @brief Creates communication services for the ServerService.
 *
 * This function sets up a Unix domain socket for communication with clients.
 * It creates a socket, binds it to a specific address, and starts listening for incoming connections.
 *
 * @return 0 if the communication services are successfully created,
 *         -1 otherwise.
 */
int ServerService::createCommunicationServices()
{
    if ((_serverFd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0)
    {
        return -1;
    }
    unlink(_socketPath.c_str());
    _address.sun_family = AF_UNIX;
    strncpy(_address.sun_path, _socketPath.c_str(), sizeof(_address.sun_path) - 1);
    if (bind(_serverFd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
    {
        return -1;
    }
    if (listen(_serverFd, 1/*backlog*/) == -1)
    {
        return -1;
    }
    return 0;
}

/**
 * @brief Destroys communication services for the ServerService.
 *
 * This function closes the server socket and removes the Unix domain socket file..
 */
void ServerService::destroyCommunicationServices()
{
    close(_serverFd);
    unlink(_socketPath.c_str());
}

/**
 * @brief Processing incomming message.
 *
 * This function parse incomming message, process it and send responce
 */
void ServerService::processMessage()
{
	Logger::redirect cout_redir(std::cout);
	
    char buffer[128];
    int numBytes = read(_clientFd, buffer, sizeof(buffer));
    cout << "Received " << numBytes << " bytes" << endl;

    if (numBytes > 0)
    {
        Header* header = reinterpret_cast<Header*>(buffer);
        cout << "Receive message:" << endl;
        cout << "Version: " << static_cast<int>(header->version) << endl;
        cout << "Seq number: " << static_cast<int>(header->seqNum) << endl;
        cout << "Message type: " << static_cast<int>(header->type) << endl;
        cout << "Message size: " << static_cast<int>(header->size) << endl;
        cout << "Message status: " << static_cast<int>(header->status) << endl;

        switch (header->type)
        {
        case MSG_PING:
        {
            cout << "Receive PING message" << endl;
            Ping resp = Transport::createPongMsg(header);
            send(_clientFd, &resp, sizeof(resp), 0);
        }
            break;
        case MSG_REQ_STATUS:
        {
            cout << "Receive REQ_STATUS message" << endl;
            Status resp = Transport::createRespStatusMsg(header, STATUS_PROCESSING);
            send(_clientFd, &resp, sizeof(resp), 0);
        }
            break;
        default:
        {
            cout << "Receive unsupported message" << endl;
            Error resp = Transport::createErrorMsg(header);
            send(_clientFd, &resp, sizeof(resp), 0);
        }
            break;
        }
    }
}

/**
 * @brief Create PID file.
 */
void ServerService::setPidFile()
{
	Logger::redirect cerr_redir(std::cerr);
	
    FILE* f;

    f = fopen(_filename.c_str(), "w+");
    if (f)
    {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
    else
    {
        cerr << "Failed to set PID to file" << endl;
    }
}

/**
 * @brief Read PID file.
 *
 * @return PID.
 */
int ServerService::getPidFile()
{
	Logger::redirect cerr_redir(std::cerr);
	
    FILE* f;
    int pid = -1;
    int res;

    f = fopen(_filename.c_str(), "r");
    if (f)
    {
        res = fscanf(f, "%d", &pid);
        if (EOF == res)
        {
            cerr << "Faied to read PID file" << endl;
        }
        fclose(f);
    }

    return pid;
}

/**
 * @brief Core function of the ServerService.
 *
 * This function waits for a client connection,
 * reads a message from the client, output it, sends a response back to the client,
 * and then closes the connection.
 *
 * @return 0 if the operation is successful, -1 otherwise.
 */
void ServerService::core()
{
	Logger::redirect cerr_redir(std::cerr);
	
    setPidFile();
    while (_loop)
    {
        _clientFd = accept(_serverFd, (struct sockaddr*)&_address, (socklen_t*)&_addrlen);
        if (_clientFd == -1)
        {
            cerr << "Failed to accept connection" << endl;
            return;
        }
        processMessage();
        close(_clientFd);
    }
    unlink(_filename.c_str());
}

/**
 * @brief Run ServerService.
 *
 * @return 0 if the operation is successful, -1 otherwise.
 */
int ServerService::run()
{
	Logger::redirect cout_redir(std::cout);
	Logger::redirect cerr_redir(std::cerr);
	
    cout << "Server was started" << endl;

    int res = createCommunicationServices();
    if (res)
    {
        cerr << "Failed to create communication service" << endl;
        return -1;
    }

    cout << "Communication service was successfully created" << endl;

    int pid = fork();
    if (pid == -1) // error
    {
        cerr << "Failed to run service" << endl;
        return -1;
    }
    else if (!pid) // create new process
    {
        umask(0);             // update permissions
        setsid();             // create new session
        chdir("/");           // change current directory
        //close(STDIN_FILENO);  // close streams
        //close(STDOUT_FILENO); // close streams
        //close(STDERR_FILENO); // close streams

        core();  // run core function
    }
    else // current process
    {
        return 0;
    }

    destroyCommunicationServices();
    cout << "Server was stoped" << endl;

    return 0;
}

/**
 * @brief Stop ServerService.
 *
 * @return 0 if the operation is successful, -1 otherwise.
 */
int ServerService::quit()
{
    int pid = getPidFile();
    kill(pid, 9);
    return 0;
}
} // namespace ORU
