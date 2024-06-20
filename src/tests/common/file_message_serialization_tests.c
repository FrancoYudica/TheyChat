#include "messages/message.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

int main()
{

    uint8_t serialized_message_buffer[1024];
    size_t serialized_message_size;

    // Creates and prints ChatMessage
    Message original_msg = create_file_header_message("test_file.txt", 512);
    print_message(&original_msg);

    // Serializes into buffer
    ns_serialize_message(&original_msg, serialized_message_buffer, &serialized_message_size);
    printf("Serialized buffer size: %ld\n", serialized_message_size);

    // Deserializes and prints
    Message deserialized_msg;
    ns_deserialize_message(serialized_message_buffer, &deserialized_msg);

    assert(original_msg.type == deserialized_msg.type);
    assert(original_msg.net_payload_length == deserialized_msg.net_payload_length);
    assert(!strcmp(original_msg.payload.file_header.name, deserialized_msg.payload.file_header.name));
    assert(original_msg.payload.file_header.size == deserialized_msg.payload.file_header.size);
    return 0;
}
