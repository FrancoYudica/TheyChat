#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <unistd.h>
#include <thread_pool.h>
#include <signal.h>
#include "server.h"
#include "messages/message.h"
#include "messages/message_types.h"
#include "net/net_communication.h"
#include "net/net_stream.h"
#include "broadcast_message.h"
#include "client_handler.h"

void handle_sigint(int sig)
{
    Error* err = server_free();
    if (IS_NET_ERROR(err))
        print_error(err);
}

int main(int argc, char** argv)
{

    uint16_t port = DEFAULT_STATUS_PORT;
    uint8_t max_client_count = 10;

    // Loads arguments
    int i = 0;
    while (++i < argc - 1) {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port")) {
            char* port_str = argv[i];
            port = atoi(port_str);
        }

        if (!strcmp(parameter, "--max_clients")) {
            char* max_clients_str = argv[i];
            max_client_count = atoi(max_clients_str);
        }

        else
            printf("Unrecognized parameter \"%s\"", parameter);
    }

    set_thread_name(pthread_self(), "main");

    // Set up the signal handler for SIGINT (CTRL+C)
    signal(SIGINT, handle_sigint);

    // Creates server with the input parameters
    Error* err = server_init(port, max_client_count);
    ASSERT_NET_ERROR(err);

    err = server_run();
    ASSERT_NET_ERROR(err);

    err = server_free();
    ASSERT_NET_ERROR(err);

    return EXIT_SUCCESS;
}
