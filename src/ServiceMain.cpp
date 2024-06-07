#include <getopt.h>

#include "ServerService.h"
#include "ClientService.h"
#include "Transport.h"

using namespace std;

/**
 * \brief List of possible services
 */
typedef enum
{
   SRV_UNKNOWN, ///< Unknown option
   SRV_SERVER,  ///< Server
   SRV_CLIENT   ///< Client
}
ServiceOption;

/**
 * \brief List of possible actions
 */
typedef enum
{
   ACT_UNKNOWN, ///< Unknown action
   ACT_RUN,     ///< Run service
   ACT_QUIT     ///< Stop service
}
ActionOption;

/**
 * \brief Arguments list
 */
typedef struct
{
    ServiceOption srv; ///< Service
    MessageType msg;   ///< Message too send (applicable for client only)
    ActionOption act;  ///< Action
}
Options;

/**
 * \brief Shows usage option selector
 */
void usage()
{
    cout << "usage: simple-service [-cshrq] [-m <type>]" << endl;
    cout << "    -s, --server" << endl;
    cout << "    -c, --client" << endl;
    cout << "    -m, --msg <type>" << endl;
    cout << "    -r, --run" << endl;
    cout << "    -q, --quit" << endl;
    cout << "    -h, --help" << endl;
}

/**
 * \brief Parsing input options
 * \param[in] argc - number of options
 * \param[in] argv - list of options
 * \retval ::InputOption
 */
void parseArgs(int argc, char** argv, Options& opt)
{
    int c = 0;

    static const struct option lopts[] = {
        {"server", 0, 0, 's'},
        {"client", 0, 0, 'c'},
        {"run", 0, 0, 'r'},
        {"quit", 0, 0, 'q'},
        {"msg", 1, 0, 'm'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}};

    while (1)
    {
        c = getopt_long(argc, argv, "cshm:rq", lopts, NULL);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'c':
            opt.srv = SRV_CLIENT;
            break;
        case 's':
            opt.srv = SRV_SERVER;
            break;
        case 'm':
            opt.msg = static_cast<MessageType>(strtoul(optarg, 0, 0));
            break;
        case 'r':
            opt.act = ACT_RUN;
            break;
        case 'q':
            opt.act = ACT_QUIT;
            break;
        case 'h':
        default:
            usage();
            exit(0);
        }
    }
}

/**
 * \brief Main.
 *
 * \details Main.
 *
 * \param[in] argc - number of options
 * \param[in] argv - list of options
 *
 * \retval Exit status
 */
int main(int argc, char* argv[])
{
    ORU::ServerService ss;
    ORU::ClientService cs;

    if (argc == 1)
    {
        usage();
        return 0;
    }
    
    Options opt = {
        SRV_UNKNOWN,
        MSG_PING,
        ACT_UNKNOWN
    };
    parseArgs(argc, argv, opt);
    if (opt.srv == SRV_SERVER)
    {
        if (opt.act == ACT_RUN)
        {
            return ss.run();
        }
        else
        {
            return ss.quit();
        }
    }
    else if (opt.srv == SRV_CLIENT)
    {
        if (opt.act == ACT_RUN)
        {
            return cs.run(opt.msg);
        }
        else
        {
            return cs.quit();
        }
    }
    else
    {
        cout << "Please specify service type [-cs]" << endl;
        usage();
    }
}
