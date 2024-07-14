#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <unistd.h>
#include <thread_pool.h>
#include "server.h"
#include "messages/message.h"
#include "messages/message_types.h"
#include "net/net_communication.h"
#include "net/net_stream.h"
#include "broadcast_message.h"
#include "client_handler.h"

int main(int argc, char** argv)
{

    uint16_t status_port = DEFAULT_STATUS_PORT;
    uint16_t task_port = DEFAULT_TASK_PORT;
    uint8_t max_client_count = 10;

    // Loads arguments
    int i = 0;
    while (++i < argc) {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--status_port")) {
            char* port_str = argv[i];
            status_port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--task_port")) {
            char* port_str = argv[i];
            task_port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--max_clients")) {
            char* max_clients_str = argv[i];
            max_client_count = atoi(max_clients_str);
        }

        else if (!strcmp(parameter, "--help") || !strcmp(parameter, "-h")) {
            printf(
                "   --status_port {port}       sets the port used in status connections\n"
                "   --task_port {port}         sets the port used in task connections\n"
                "   --max_clients {max_uint}   sets the maximum amount of connected clients\n");
            return EXIT_SUCCESS;
        }

        else {
            printf("Unrecognized parameter \"%s\". Use \"-h\" or \"--help\" to see all accepted arguments\n", parameter);
            return EXIT_SUCCESS;
        }
    }

    set_thread_name(pthread_self(), "main");

    // Creates server with the input parameters
    Error* err = server_init(
        status_port,
        task_port,
        max_client_count);

    ASSERT_NET_ERROR(err);

    err = server_run();
    ASSERT_NET_ERROR(err);

    err = server_free();
    ASSERT_NET_ERROR(err);

    return EXIT_SUCCESS;
}
