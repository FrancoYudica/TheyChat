#include "net_stream.h"
#include "net/serialization/net_message_serializer.h"


bool pop_message_from_stream(Message** message, NetworkStream* ns)
{

    uint32_t base_message_size = sizeof((*message)->base.type) + sizeof((*message)->base.payload_length);

    // Stream doesn't have the header
    if (ns->written_bytes < base_message_size)
        return false;

    // Gets the type and payload length of the message that is on top of the TCP queue
    uint32_t message_payload_length = ns_access_message_length(ns->stream);

    // The expected message size are the 2 constant size fields, plus the payload length
    uint32_t message_size = base_message_size + message_payload_length;

    // When there are remaining bytes
    if (ns->written_bytes < message_size)
        return false;

    // Allocates memory for the message
    *message = (Message*)malloc(message_size);

    // Deserializes the message from the buffer
    ns_deserialize_message(ns->stream, *message);

    // Moves network stream data, so it can keep adding more data
    // @todo memmove
    memcpy(
        ns->stream, 
        ns->stream + message_size, 
        sizeof(ns->stream) - message_size
    );

    ns->written_bytes -= message_size;
    return true;
}
