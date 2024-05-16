#include "message.h"
#include "message_serializer.h"
#include "message_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int main()
{   

    uint8_t serialized_message_buffer[1024];
    size_t serialized_message_size;

    // Creates and prints ChatMessage
    FileInfoMsg original_msg;
    init_file_info_message(&original_msg, "test_file.txt", 512);
    print_message((Message*)&original_msg);

    // Serializes into buffer
    serialize_message((Message*)&original_msg, serialized_message_buffer, &serialized_message_size);
    printf("Serialized buffer size: %ld\n", serialized_message_size);

    // Deserializes and prints
    FileInfoMsg deserialized_msg;
    deserialize_message(serialized_message_buffer, (Message*)&deserialized_msg);

    bool correct = true;
    correct &= original_msg.base.type == deserialized_msg.base.type;
    correct &= original_msg.base.payload_length == deserialized_msg.base.payload_length;
    correct &= !strcmp(original_msg.name, deserialized_msg.name);
    correct &= original_msg.size == deserialized_msg.size;
    return !correct;

    
}
