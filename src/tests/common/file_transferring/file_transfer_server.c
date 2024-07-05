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

int main()
{
    if (IS_NET_ERROR(server_init_network()))
        return 1;

    Error* err = net_listen(net_connection.context, 5);
    if (IS_NET_ERROR(err)) {
        printf("%s\n", err->message);
        return 1;
    }

    printf("Listening on port %i. Waiting for client connections...\n", port);

    NetworkConnection client_connection;
    err = net_accept_connection(net_connection.context, &client_connection.context);

    if (IS_NET_ERROR(err)) {
        printf("%s\n", err->message);
        return 1;
    }
    const char* filepath = "pdf_file.pdf";

    printf("Client connected! Sending file...\n");
    err = send_file(filepath, &client_connection);
    if (IS_NET_ERROR(err)) {
        printf("%s\n", err->message);
        return 1;
    }

    net_shutdown();
    return 0;
}