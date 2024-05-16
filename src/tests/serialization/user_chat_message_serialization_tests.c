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
    char chat_text[] = "Hey! How are you?";
    UserChatMsg chat_message;
    init_user_chat_msg(&chat_message, chat_text, "franco");
    print_message((Message*)&chat_message);

    // Serializes into buffer
    printf("SERIALIZING CHAT MESSAGE\n");
    serialize_message((Message*)&chat_message, serialized_message_buffer, &serialized_message_size);
    printf("Serialized buffer size: %ld\n", serialized_message_size);

    // Deserializes and prints
    printf("DESERIALIZING CHAT MESSAGE\n");
    UserChatMsg deserialized_chat;
    deserialize_message(serialized_message_buffer, (Message*)&deserialized_chat);
    print_message((Message*)&deserialized_chat);

    bool correct = true;
    correct &= chat_message.user_base.base.type == deserialized_chat.user_base.base.type;
    correct &= chat_message.user_base.base.payload_length == deserialized_chat.user_base.base.payload_length;
    correct &= !strcmp(chat_message.user_base.username, deserialized_chat.user_base.username);
    correct &= !strcmp(chat_message.text, deserialized_chat.text);
    
    return !correct;

    
}
