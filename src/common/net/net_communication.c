#include <stdbool.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "net/net_primitives.h"
#include "net/net_communication.h"
#include "net/serialization/net_message_serializer.h"

Error* send_message(
    const Message* msg,
    NetworkConnection* net_connection)
{
    // Allocates memory for serialized message
    uint8_t serialized_message[MAX_MESSAGE_NET_SIZE];
    size_t serialized_buffer_size;

    // Serializes the message
    ns_serialize_message(msg, serialized_message, &serialized_buffer_size);

    // Sends serialized buffer to socket
    uint32_t total_bytes_sent = 0;

    // Sends until the entire buffer is sent
    while (total_bytes_sent < serialized_buffer_size) {
        uint32_t bytes_sent;

        Error* err = net_send(
            net_connection->context,
            serialized_message + total_bytes_sent,
            serialized_buffer_size - total_bytes_sent,
            &bytes_sent);

        if (IS_NET_ERROR(err))
            return err;

        total_bytes_sent += bytes_sent;
    }

    return CREATE_ERR_OK;
}

/// @brief Receives bytes from a network socket and writes them into a network stream.
/// @param network_stream Pointer to the NetworkStream to write the received bytes into.
/// @param sockfd The file descriptor of the socket to receive bytes from.
/// @return status
static Error* receive(NetworkConnection* connection)
{
    // Pointer to the free space
    NetworkStream* network_stream = &connection->stream;
    uint8_t* empty_region_ptr = network_stream->stream + network_stream->written_bytes;

    // Amount of free space in bytes
    uint32_t empty_region_size = sizeof(network_stream->stream) - network_stream->written_bytes;

    if (empty_region_size == 0) {
        char err_msg[128];
        sprintf(
            err_msg,
            "NetworkStream overflow while waiting for message. NetworkStream of size: %ld",
            sizeof(network_stream->stream));

        return CREATE_ERR(ERR_NET_STREAM_OVERFLOW, err_msg);
    }

    // Blocks current thread until it receives data
    uint32_t bytes_read;
    Error* err = net_receive(
        connection->context,
        empty_region_ptr,
        empty_region_size,
        &bytes_read);

    if (IS_NET_ERROR(err))
        return err;

    network_stream->written_bytes += bytes_read;

    return CREATE_ERR_OK;
}

Error* wait_for_message(
    NetworkConnection* connection,
    Message* message)
{

    // Tries to pop message
    bool popped = stream_pop_message(&connection->stream, message);

    // If no messages are popped
    // Receives data until a complete message arrives
    while (!popped) {
        Error* err = receive(connection);
        if (IS_NET_ERROR(err))
            return err;

        popped = stream_pop_message(&connection->stream, message);
    }

    return CREATE_ERR_OK;
}

Error* wait_for_message_type(
    NetworkConnection* connection,
    Message* message,
    enum MessageType type)
{

    Error* err = wait_for_message(connection, message);
    if (IS_NET_ERROR(err))
        return err;

    if (message->type != type) {

        char err_log[128];
        sprintf(
            err_log,
            "Received unexpected message type. Expected type %s but received %s",
            msg_get_type_name(type),
            msg_get_type_name(message->type));

        return CREATE_ERR(ERR_NET_RECEIVED_INVALID_TYPE, err_log);
    }
    return CREATE_ERR_OK;
}
