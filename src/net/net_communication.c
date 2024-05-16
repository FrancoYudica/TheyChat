#include "net_communication.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include "serialization/net_message_serializer.c"

static void send_buffer_to_socketfd(const uint8_t *buffer, size_t buffer_size, uint32_t socketfd)
{
    if (send(socketfd, buffer, buffer_size, 0) == -1)
    {
        perror("`send` in `send_buffer_to_socketfd failed`");
        exit(1);
    }
}

void send_message_to_socketfd(const Message *msg, uint32_t socketfd)
{
    // Allocates memory for serialized message
    uint8_t serialized_message[1024];
    size_t serialized_buffer_size;

    // Serializes the message
    ns_serialize_message(msg, serialized_message, &serialized_buffer_size);

    // Sends serialized buffer to socket
    send_buffer_to_socketfd(serialized_message, serialized_buffer_size, socketfd);
}