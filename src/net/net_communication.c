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

void receive_message_from_socketfd(Message** message, uint32_t sockfd)
{
    uint8_t receive_buffer[1024];

    // Receive response from server
    int bytes_read = recv(sockfd, receive_buffer, sizeof(receive_buffer), 0);

    if (bytes_read < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Connection closed by the peer
    if (bytes_read == 0)
    {
        printf("Connection closed by peer\n");
        exit(EXIT_SUCCESS);
    }
    // Data successfully read from the socket
    // Access message type field and allocates memory for the type
    uint8_t message_type = ns_access_message_type(receive_buffer);
    printf("Message type: %i\n", message_type);
    // @todo change this
    uint32_t message_size = sizeof(UserChatMsg);

    // Allocates memory for the message type
    *message = (Message*)malloc(message_size);

    // Deserializes onto message
    ns_deserialize_message(receive_buffer, *message);
}
