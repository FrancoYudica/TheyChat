#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 512
#define MESSAGE_TYPE_SIZE sizeof(uint8_t)
#define MESSAGE_LENGTH_SIZE sizeof(size_t)
#define MESSAGE_PAYLOAD_SIZE (MESSAGE_SIZE - MESSAGE_TYPE_SIZE - MESSAGE_LENGTH_SIZE)

enum MessageTypes
{
    MESSAGE_TYPE_CHAT,
    MESSAGE_TYPE_LOGIN,
    MESSAGE_TYPE_LOGOUT,
    MESSAGE_TYPE_FILE_INFO
};


#define MAX_CHAT_TEXT_BYTES 256
#define MAX_USERNAME_BYTES 32

/// @brief Data shared by all messages
typedef struct
{
    uint8_t type;
    uint32_t payload_length;
} BaseMessage;

/// @brief Message basic data layout
typedef struct 
{
    BaseMessage base;
} Message;


typedef struct 
{
    BaseMessage base;
    char username[MAX_USERNAME_BYTES];
    char text[MAX_CHAT_TEXT_BYTES];
} ChatMessage;


/// @brief Stores descriptive data of the file
typedef struct
{
    BaseMessage base;

    /// @brief Name of the file
    char name[64];

    /// @brief Size of the file in bytes
    uint32_t size;

} FileInfoMessage;


/// @brief Copies the atribute in the buffer, and offsets buffer pointer by the attribute size
/// @param buffer_ptr Buffer where attribute is copied
/// @param attrib Copied attribute
/// @param attrib_size Size in bytes of attribute
void push_attribute(char **buffer_ptr, const void *attrib, size_t attrib_size)
{
    memcpy(*buffer_ptr, attrib, attrib_size);
    *buffer_ptr += attrib_size;
}

void push_long(char **buffer_ptr, const uint32_t *n, size_t attrib_size)
{
    // Transforms host long to net long
    uint32_t net_n = htonl(*n);
    push_attribute(buffer_ptr, &net_n, attrib_size);
}

void push_short(char **buffer_ptr, const uint16_t *n, size_t attrib_size)
{
    // Transforms host long to net long
    uint16_t net_n = htons(*n);
    push_attribute(buffer_ptr, &net_n, attrib_size);
}


/// @brief Copies the atribute from the buffer, and offsets buffer pointer by the attribute size
/// @param buffer_ptr Buffer that holds the attribute
/// @param attrib Pointer to attribute
/// @param attrib_size Size in bytes of attribute
void pop_attribute(char **buffer_ptr, void *atrib, size_t attrib_size)
{
    // Copies the atribute
    memcpy(atrib, *buffer_ptr, attrib_size);
    *buffer_ptr += attrib_size;
}

void pop_long(char **buffer_ptr, uint32_t *n, size_t attrib_size)
{
    // Pops network long
    uint32_t net_n;
    pop_attribute(buffer_ptr, &net_n, attrib_size);

    // Transforms network long to host long
    *n = ntohl(net_n);
}

void pop_short(char **buffer_ptr, uint16_t *n, size_t attrib_size)
{
    // Pops network long
    uint16_t net_n;
    pop_attribute(buffer_ptr, &net_n, attrib_size);

    // Transforms network long to host long
    *n = ntohs(net_n);
}


void serialize_message(const Message *message, char *buffer, size_t *buffer_size)
{
    char* buffer_ptr = buffer;

    // Serializes message type
    push_attribute(&buffer_ptr, &message->base.type, sizeof(message->base.type));
    push_long(&buffer_ptr, &message->base.payload_length, sizeof(message->base.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->base.type)
    {
    case MESSAGE_TYPE_CHAT:
        ChatMessage *chat_message = (ChatMessage*)message;
        push_attribute(&buffer_ptr, chat_message->username, MAX_USERNAME_BYTES);
        push_attribute(&buffer_ptr, chat_message->text, MAX_CHAT_TEXT_BYTES);
        break;
    
    case MESSAGE_TYPE_FILE_INFO:
        FileInfoMessage *file_message = (FileInfoMessage*)message;
        push_attribute(&buffer_ptr, file_message->name, sizeof(file_message->name));
        push_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
        break;

    
    default:
        break;
    }

    // Buffer size is calculated as the difference between the pointers
    *buffer_size = buffer_ptr - buffer;
}

void deserialize_message(const char* buffer, Message *message)
{
    char* buffer_ptr = (char*)buffer;

    // Pops type
    pop_attribute(&buffer_ptr, &message->base.type, sizeof(message->base.type));

    // Pops payload length
    pop_long(&buffer_ptr, &message->base.payload_length, sizeof(message->base.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->base.type)
    {
    case MESSAGE_TYPE_CHAT:
        ChatMessage *chat_message = (ChatMessage*)message;
        pop_attribute(&buffer_ptr, chat_message->username, MAX_USERNAME_BYTES);
        pop_attribute(&buffer_ptr, chat_message->text, MAX_CHAT_TEXT_BYTES);
        break;

    case MESSAGE_TYPE_FILE_INFO:
        FileInfoMessage *file_message = (FileInfoMessage*)message;
        pop_attribute(&buffer_ptr, file_message->name, sizeof(file_message->name));
        pop_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
        break;

    default:
        break;
    }
}


#endif