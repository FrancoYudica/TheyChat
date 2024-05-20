#include "net_stream.h"
#include "net/serialization/net_message_serializer.h"
#include "message_types.h"

static uint8_t ns_access_message_type(const uint8_t *buffer);
static uint32_t ns_access_message_length(const uint8_t *buffer);


void init_net_stream(NetworkStream* ns)
{
    ns->written_bytes = 0;
    memset(ns, 0, sizeof(NetworkStream));
}

Message* stream_pop_message(NetworkStream* ns)
{
    static uint32_t base_message_size = 5;

    // Stream doesn't have the header
    if (ns->written_bytes < base_message_size)
        return NULL;

    // Gets the type and payload length of the message that is on top of the TCP queue
    uint32_t message_payload_length = ns_access_message_length(ns->stream);

    // The expected message size are the 2 constant size fields, plus the payload length
    // note that this is not the actual message size in memory, since network stream
    // doesn't hold padding
    uint32_t net_message_size = base_message_size + message_payload_length;

    // When there are remaining bytes
    if (ns->written_bytes < net_message_size)
        return NULL;

    // Access message type and gets size
    uint8_t message_type = ns_access_message_type(ns->stream);
    uint32_t type_bytes_size = msg_get_type_size(message_type);
    
    // Allocates memory for the message, taking in consideration padding
    Message *message = (Message*)malloc(type_bytes_size);

    // Deserializes the message from the buffer
    ns_deserialize_message(ns->stream, message);

    // Moves network stream data, so it can keep adding more data
    memmove(
        ns->stream,
        ns->stream + net_message_size,
        sizeof(ns->stream) - net_message_size
    );

    ns->written_bytes -= net_message_size;
    return message;
}

static uint8_t ns_access_message_type(const uint8_t *buffer)
{
    return buffer[0];
}

static uint32_t ns_access_message_length(const uint8_t *buffer)
{
    uint32_t net_length;
    memcpy(&net_length, buffer + sizeof(uint8_t), sizeof(uint32_t));

    uint32_t host_length = ntohl(net_length);
    return host_length;
}
