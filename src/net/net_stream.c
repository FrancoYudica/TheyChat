#include "net_stream.h"
#include "net/serialization/net_message_serializer.h"
#include "messages/messages.h"

void init_net_stream(NetworkStream* ns)
{
    ns->written_bytes = 0;
    memset(ns, 0, sizeof(NetworkStream));
}

bool stream_pop_message_by_type(uint8_t expected_type, Message** message, NetworkStream* ns)
{
    if (!stream_pop_message(message, ns))
        return false;

    if ((*message)->base.type != expected_type)
    {
        printf("ERROR: Received unexpected message type. Expected %i, and received %i\n", expected_type, (*message)->base.type);
        printf("Received message: ");
        print_message(*message);
        exit(EXIT_FAILURE);
    }

    return true;
}

bool stream_pop_message(Message** message, NetworkStream* ns)
{

    uint32_t base_message_size = sizeof((*message)->base.type) + sizeof((*message)->base.payload_length);

    // Stream doesn't have the header
    if (ns->written_bytes < base_message_size)
        return false;

    // Gets the type and payload length of the message that is on top of the TCP queue
    uint32_t message_payload_length = ns_access_message_length(ns->stream);

    // The expected message size are the 2 constant size fields, plus the payload length
    // note that this is not the actual message size in memory, since network stream
    // doesn't hold padding
    uint32_t net_message_size = base_message_size + message_payload_length;

    // When there are remaining bytes
    if (ns->written_bytes < net_message_size)
        return false;

    // Access message type and gets size
    uint8_t message_type = ns_access_message_type(ns->stream);
    uint32_t type_bytes_size = msg_get_type_size(message_type);
    
    // Allocates memory for the message, taking in consideration padding
    *message = (Message*)malloc(type_bytes_size);

    // Deserializes the message from the buffer
    ns_deserialize_message(ns->stream, *message);

    // Moves network stream data, so it can keep adding more data
    memmove(
        ns->stream,
        ns->stream + net_message_size,
        sizeof(ns->stream) - net_message_size
    );

    ns->written_bytes -= net_message_size;
    return true;
}
