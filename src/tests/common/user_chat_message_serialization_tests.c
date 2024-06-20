#include "messages/message.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

int main()
{

    // Creates and prints ChatMessage
    Message chat_message = create_user_chat_msg("Hey! How are you?", "franco");
    print_message(&chat_message);

    // Serializes into buffer
    printf("SERIALIZING CHAT MESSAGE\n");
    uint8_t serialized_message_buffer[1024];
    size_t serialized_message_size;
    ns_serialize_message(&chat_message, serialized_message_buffer, &serialized_message_size);
    printf("Serialized buffer size: %ld\n", serialized_message_size);

    // Deserializes and prints
    printf("DESERIALIZING CHAT MESSAGE\n");
    Message deserialized_chat;
    ns_deserialize_message(serialized_message_buffer, &deserialized_chat);
    print_message(&deserialized_chat);

    assert(chat_message.type == deserialized_chat.type);
    assert(chat_message.net_payload_length == deserialized_chat.net_payload_length);
    assert(!strcmp(chat_message.payload.user_chat.username, deserialized_chat.payload.user_chat.username));
    assert(!strcmp(chat_message.payload.user_chat.text, deserialized_chat.payload.user_chat.text));

    return 0;
}
