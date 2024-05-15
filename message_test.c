#include "message.h"
#include <stdio.h>
#include <stdlib.h>

void print_message(Message *message)
{
    // Maps numeric type to string
    static char* message_types[] = {
        "CHAT",
        "LOGIN",
        "LOGOUT",
        "FILE"
    };

    // Prints type and length
    printf("MESSAGE{type: %s, length: %d, ", message_types[message->base.type], message->base.payload_length);

    // Prints specific data to each message
    switch (message->base.type)
    {
    case MESSAGE_TYPE_CHAT:
        ChatMessage *chat_message = (ChatMessage*)message;
        printf("username: %s, text: %s}\n", chat_message->username, chat_message->text);
        break;
    
    case MESSAGE_TYPE_FILE_INFO:
        FileInfoMessage *file_info_message = (FileInfoMessage*)message;
        printf("name: %s, size: %d}\n", file_info_message->name, file_info_message->size);
        break;

    case MESSAGE_TYPE_LOGIN:
        printf("type: LOGIN, ");
        break;

    case MESSAGE_TYPE_LOGOUT:
        printf("type: LOGOUT, ");
        break;

    default:
        break;
    }
}


void init_chat_message(ChatMessage *message, const char* text, const char* username)
{
    // Initializes message data
    message->base.type = MESSAGE_TYPE_CHAT;
    message->base.payload_length = sizeof(message->text) + sizeof(message->username);

    strcpy(message->text, text);
    strcpy(message->username, username);
}


void init_file_info_message(FileInfoMessage *message, const char* name, uint32_t size)
{
    // Initializes message data
    message->base.type = MESSAGE_TYPE_FILE_INFO;
    message->base.payload_length = sizeof(message->name) + sizeof(message->size);

    strcpy(message->name, name);
    message->size = size;
}


int main()
{

    // ChatMessage - Serialization - Deserialization test
    {
        printf("CHAT MESSAGE TEST ------------------------------------------------------------------------\n");
        char serialized_message_buffer[1024];
        size_t serialized_message_size;

        {
            // Creates and prints ChatMessage
            printf("CREATING CHAT MESSAGE\n");
            char chat_text[] = "Hey! How are you?";
            ChatMessage chat_message;
            init_chat_message(&chat_message, chat_text, "franco");
            print_message((Message*)&chat_message);

            // Serializes into buffer
            printf("SERIALIZING CHAT MESSAGE\n");
            serialize_message((Message*)&chat_message, serialized_message_buffer, &serialized_message_size);
            printf("Serialized buffer size: %ld\n", serialized_message_size);
        }

        {
            // Deserializes and prints
            printf("DESERIALIZING CHAT MESSAGE\n");
            ChatMessage deserialized_chat;
            deserialize_message(serialized_message_buffer, (Message*)&deserialized_chat);
            print_message((Message*)&deserialized_chat);
        }
    }

    // ChatMessage - Serialization - Deserialization test
    {
        printf("FILE INFO MESSAGE TEST -------------------------------------------------------------------\n");
        char serialized_message_buffer[1024];
        size_t serialized_message_size;

        {
            // Creates and prints ChatMessage
            printf("CREATING FILE INFO MESSAGE\n");
            FileInfoMessage file_info_message;
            init_file_info_message(&file_info_message, "test_file.txt", 512);
            print_message((Message*)&file_info_message);

            // Serializes into buffer
            printf("SERIALIZING FILE INFO MESSAGE\n");
            serialize_message((Message*)&file_info_message, serialized_message_buffer, &serialized_message_size);
            printf("Serialized buffer size: %ld\n", serialized_message_size);
        }

        {
            // Deserializes and prints
            printf("DESERIALIZING FILE INFO MESSAGE\n");
            FileInfoMessage file_info_message;
            deserialize_message(serialized_message_buffer, (Message*)&file_info_message);
            print_message((Message*)&file_info_message);
        }
    }

    
}
