#include "net/file_transfer.h"

uint16_t port = 10000;
NetworkConnection net_connection;

Error* server_init_network()
{

    // Initializes network module.
    net_init();
    Error* err;

#ifdef THEY_CHAT_SSL
    printf("Loading SSL certificate and key\n");
    // Certificate and private key paths
    const char* home_path = getenv("HOME");
    char cert_file[128];
    char key_file[128];
    sprintf(cert_file, "%s/.ssl/TheyChat/certificate.pem", home_path);
    sprintf(key_file, "%s/.ssl/TheyChat/private.key", home_path);

    printf("- Certificate path: %s\n", cert_file);
    printf("- Private key path: %s\n", key_file);
    // Initializes socket with certificates
    err = net_server_create_socket(cert_file, key_file, port, &net_connection.context);

#else
    // Not using SSL, certificates not needed
    err = net_server_create_socket(NULL, NULL, port, &net_connection.context);
#endif
    return err;
}

void check_error(Error* err)
{
    if (IS_ERROR(err)) {
        printf("%s\n", err->message);
        free_network_connection(&net_connection);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv)
{
    char server_ip[16];
    if (argc > 1)
        strcpy(server_ip, argv[1]);
    else {
        printf("Should specify IP\n");
        return 0;
    }

    init_network_connection(&net_connection);

    printf(
        "Connecting to server ip (%s) and port (%d)...\n",
        server_ip,
        port);

    net_init();

    // Establishes connection with serer
    Error* err = net_client_create_socket(
        port,
        server_ip,
        &net_connection.context);

    check_error(err);

    err = receive_file(&net_connection, NULL, NULL, NULL);

    check_error(err);

    net_shutdown();
    return 0;
}