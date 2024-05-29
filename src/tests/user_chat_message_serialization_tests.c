#include "messages/message.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main()
{

    // Creates and prints ChatMessage
    UserChatMsg* chat_message = create_user_chat_msg("Hey! How are you?", "franco");
    print_message((Message*)chat_message);

    // Serializes into buffer
    printf("SERIALIZING CHAT MESSAGE\n");
    uint8_t serialized_message_buffer[1024];
    size_t serialized_message_size;
    ns_serialize_message((Message*)chat_message, serialized_message_buffer, &serialized_message_size);
    printf("Serialized buffer size: %ld\n", serialized_message_size);

    // Deserializes and prints
    printf("DESERIALIZING CHAT MESSAGE\n");
    UserChatMsg deserialized_chat;
    ns_deserialize_message(serialized_message_buffer, (Message*)&deserialized_chat);
    print_message((Message*)&deserialized_chat);

    bool correct = true;
    correct &= chat_message->user_base.header.type == deserialized_chat.user_base.header.type;
    correct &= chat_message->user_base.header.payload_length == deserialized_chat.user_base.header.payload_length;
    correct &= !strcmp(chat_message->user_base.username, deserialized_chat.user_base.username);
    correct &= !strcmp(chat_message->text, deserialized_chat.text);

    return !correct;
}
