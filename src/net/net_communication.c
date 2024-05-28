#include <stdbool.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "net_communication.h"
#include "serialization/net_message_serializer.h"


ErrorCode send_message(const Message *msg, uint32_t socketfd)
{
    // Allocates memory for serialized message
    uint8_t serialized_message[1024];
    size_t serialized_buffer_size;

    // Serializes the message
    ns_serialize_message(msg, serialized_message, &serialized_buffer_size);

    // Sends serialized buffer to socket
    uint32_t total_bytes_sent = 0;

    // Sends until the entire buffer is sent
    while(total_bytes_sent < serialized_buffer_size)
    {
        int32_t bytes_sent = send(socketfd, serialized_message + total_bytes_sent, serialized_buffer_size - total_bytes_sent, 0);

        if (bytes_sent == -1)
        {
            perror("`send` in `send_buffer_to_socketfd failed`");
            return ERR_SEND_FAIL;
        }
        total_bytes_sent += bytes_sent;
    }
    
    return ERR_NET_OK;
}

/// @brief Receives bytes from a network socket and writes them into a network stream.
/// @param network_stream Pointer to the NetworkStream to write the received bytes into.
/// @param sockfd The file descriptor of the socket to receive bytes from.
/// @return status
static ErrorCode receive(NetworkStream* network_stream, uint32_t sockfd)
{
    // Pointer to the free space
    uint8_t *empty_region_ptr = network_stream->stream + network_stream->written_bytes;

    // Amount of free space in bytes
    uint32_t empty_region_size = sizeof(network_stream->stream) - network_stream->written_bytes;

    if (empty_region_size == 0)
    {
        printf("WARNING: Trying to receive data in a full NetworkStream...\n");
        return ERR_NET_STREAM_OVERFLOW;
    }

    // Blocks current thread until it receives data
    int32_t bytes_read = recv(sockfd, empty_region_ptr, empty_region_size, 0);
    
    if (bytes_read == -1)
    {
        return ERR_RECEIVE_FAIL;
    }
    if (bytes_read == 0)
    {
        return ERR_PEER_DISCONNECTED;
    }

    network_stream->written_bytes += bytes_read;

    return ERR_NET_OK;
}

ErrorCode wait_for_message(NetworkStream* network_stream, uint32_t sockfd, Message **message)
{

    // Tries to pop message
    *message = stream_pop_message(network_stream);
    
    // If no messages are popped
    // Receives data until a complete message arrives
    while (*message == NULL)
    {
        ErrorCode receive_status = receive(network_stream, sockfd);
        if (IS_NET_ERROR(receive_status))
            return receive_status;

        *message = stream_pop_message(network_stream);
    }

    return ERR_NET_OK;
}

ErrorCode wait_for_message_type(NetworkStream* network_stream, uint32_t sockfd, Message **message, uint8_t type)
{

    ErrorCode status = wait_for_message(network_stream, sockfd, message);
    if (IS_NET_ERROR(status))
        return status;

    if ((*message)->header.type != type)
    {
        printf("ERROR: Received unexpected message type. Expected %i, and received %i. ", type, (*message)->header.type);
        printf("Received message: ");
        print_message(*message);

        return ERR_RECEIVED_INVALID_TYPE;
    }
    return ERR_NET_OK;
}
