#include "server.h"
#include "file.h"
#include "messages/message_types.h"
#include "broadcast_message.h"
#include "client_handler.h"
#include "net/net_communication.h"

static Server s_server;
static bool s_running = false;

Server* get_server()
{
    return &s_server;
}

Error* server_init(uint16_t port, uint32_t max_client_count)
{
    memset(&s_server, 0, sizeof(Server));

    // Initializes server data
    s_server.port = port;
    s_server.max_client_count = max_client_count;

    s_server.client_list = client_list_create();
    s_server.shared_file_list = shared_file_list_create();

    pthread_mutex_init(&s_server.client_list_mutex, NULL);
    pthread_mutex_init(&s_server.shared_file_list_mutex, NULL);
    pthread_mutex_init(&s_server.broadcast_mutex, NULL);

    s_server.client_thread_pool = thpool_create(max_client_count);
    s_server.task_thread_pool = thpool_create(max_client_count);

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
    err = net_server_create_socket(cert_file, key_file, s_server.port, &s_server.context);

#else
    // Not using SSL, certificates not needed
    err = net_server_create_socket(NULL, NULL, s_server.port, &s_server.context);
#endif
    return err;
}

Error* server_run()
{
    // Sets up listen
    Error* err = net_listen(s_server.context, 5);
    if (IS_ERROR(err))
        return err;

    printf("Listening on port %i. Waiting for client connections...\n", s_server.port);

    // Message sent to client in case there aren't any free
    // threads, and stays in client queue
    Message client_on_queue_msg = create_client_on_queue();
    s_running = true;
    while (s_running) {

        // Accepts client connections
        ConnectionContext* client_status_context = NULL;
        err = net_accept_connection(s_server.context, &client_status_context);

        if (IS_ERROR(err)) {

            // When application ins't running, the error means that
            // the connection was closed
            if (!s_running)
                err = CREATE_ERR_OK;

            break;
        }

        printf("Accepted client status connection\n");

        ConnectionContext* client_task_context = NULL;
        err = net_accept_connection(s_server.context, &client_task_context);

        if (IS_ERROR(err))
            break;

        printf("Accepted client task connection\n");

        // Registers client
        pthread_mutex_lock(&s_server.client_list_mutex);
        Client* client = client_list_add(s_server.client_list);
        pthread_mutex_unlock(&s_server.client_list_mutex);

        // Initializes client network data
        init_client_network(client, client_status_context, client_task_context);

        // Tells client that all threads are busy, and it's on queue
        if (thpool_all_threads_busy(s_server.client_thread_pool))
            send_message((Message*)&client_on_queue_msg, &client->status_connection);

        // Creates handler data and queues a new task
        ClientHandlerData* handler_data = (ClientHandlerData*)malloc(sizeof(ClientHandlerData));
        handler_data->client = client;
        thpool_submit(s_server.client_thread_pool, (thread_task_t)handle_client_task, handler_data);
    }
    return err;
}

static void remove_all_shared_files()
{
    Error* err = CREATE_ERR_OK;
    Server* server = get_server();

    SharedFile* file = NULL;
    SharedFileListIterator* it = shared_file_list_iterator_create(server->shared_file_list);

    // Removes all the files that belong to the client
    while ((file = shared_file_list_iterator_next(it))) {

        // Tries to remove
        err = server_remove_shared_file(file->id);

        // Displays error
        if (IS_ERROR(err)) {
            print_error(err);
            free_error(err);
            err = CREATE_ERR_OK;
        }
    }

    shared_file_list_iterator_destroy(it);
}

Error* server_free()
{

    if (!s_running)
        return CREATE_ERR_OK;

    s_running = false;

    printf("\nFreeing server memory\n");

    Error* err = net_close(s_server.context);
    printf("    - Context closed\n");

    thpool_destroy(s_server.client_thread_pool);
    printf("    - Client thpool destroyed\n");

    thpool_destroy(s_server.task_thread_pool);
    printf("    - Task thpool destroyed\n");

    client_list_destroy(s_server.client_list);
    printf("    - Client list destroyed\n");

    remove_all_shared_files();
    printf("    - All shared files removed from disk\n");

    shared_file_list_destroy(s_server.shared_file_list);
    printf("    - Shared file list destroyed\n");

    pthread_mutex_destroy(&s_server.client_list_mutex);
    pthread_mutex_destroy(&s_server.shared_file_list_mutex);
    pthread_mutex_destroy(&s_server.broadcast_mutex);
    printf("    - Mutexes destroyed\n");
    net_shutdown();
    printf("    - Network shutdown\n");

    return err;
}

void server_client_count_update()
{
    // Sends to all clients the currently connected clients
    uint32_t connected_client_count = client_list_length(s_server.client_list);
    Message connected_clients = create_connected_clients_msg(connected_client_count);
    send_broadcast((const Message*)&connected_clients, get_server());
}

Error* server_remove_shared_file(uint32_t id)
{
    // Finds in list
    SharedFile* file = shared_file_list_find_by_id(s_server.shared_file_list, id);
    if (file == NULL)
        return CREATE_ERR(ERR_FILE, "Unable to find specified file id in shared file list");

    // Removes from disk
    Error* err = shared_file_remove_from_disk(file);
    if (IS_ERROR(err))
        return err;

    // Removes from list
    bool removed = shared_file_list_remove(s_server.shared_file_list, file->id);
    if (!removed)
        return CREATE_ERR(ERR_FILE, "Trying to remove file from shared_file_list, when it doesn't exists");

    return CREATE_ERR_OK;
}
