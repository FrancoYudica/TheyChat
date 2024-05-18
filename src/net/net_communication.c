#include <stdbool.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "net_communication.h"
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


void receive(NetworkStream* network_stream, uint32_t sockfd)
{
    // Pointer to the free space
    uint8_t *empty_region_ptr = network_stream->stream + network_stream->written_bytes;

    // Amount of free space in bytes
    uint32_t empty_region_size = sizeof(network_stream->stream) - network_stream->written_bytes;

    if (empty_region_size == 0)
    {
        printf("WARNING: Trying to receive data in a full NetworkStream...\n");
        return;
    }

    // Blocks current thread until it receives data
    uint32_t bytes_read = recv(sockfd, empty_region_ptr, empty_region_size, 0);

    if (bytes_read == -1)
    {
        perror("recv error");
        exit(EXIT_FAILURE);
    }
    
    if (bytes_read == 0)
    {
        printf("Connection closed by peer\n");
        exit(EXIT_SUCCESS);
    }

    network_stream->written_bytes += bytes_read;
}
