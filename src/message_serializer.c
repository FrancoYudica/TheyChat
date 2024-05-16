#include "message_serializer.h"
#include "message_types.h"

/// @brief Copies the atribute in the buffer, and offsets buffer pointer by the attribute size
/// @param buffer_ptr Buffer where attribute is copied
/// @param attrib Copied attribute
/// @param attrib_size Size in bytes of attribute
static void push_attribute(uint8_t **buffer_ptr, const void *attrib, size_t attrib_size)
{
    memcpy(*buffer_ptr, attrib, attrib_size);
    *buffer_ptr += attrib_size;
}

static void push_long(uint8_t **buffer_ptr, const uint32_t *n, size_t attrib_size)
{
    // Transforms host long to net long
    uint32_t net_n = htonl(*n);
    push_attribute(buffer_ptr, &net_n, attrib_size);
}

static void push_short(uint8_t **buffer_ptr, const uint16_t *n, size_t attrib_size)
{
    // Transforms host long to net long
    uint16_t net_n = htons(*n);
    push_attribute(buffer_ptr, &net_n, attrib_size);
}


/// @brief Copies the atribute from the buffer, and offsets buffer pointer by the attribute size
/// @param buffer_ptr Buffer that holds the attribute
/// @param attrib Pointer to attribute
/// @param attrib_size Size in bytes of attribute
static void pop_attribute(uint8_t **buffer_ptr, void *atrib, size_t attrib_size)
{
    // Copies the atribute
    memcpy(atrib, *buffer_ptr, attrib_size);
    *buffer_ptr += attrib_size;
}

static void pop_long(uint8_t **buffer_ptr, uint32_t *n, size_t attrib_size)
{
    // Pops network long
    uint32_t net_n;
    pop_attribute(buffer_ptr, &net_n, attrib_size);

    // Transforms network long to host long
    *n = ntohl(net_n);
}

static void pop_short(uint8_t **buffer_ptr, uint16_t *n, size_t attrib_size)
{
    // Pops network long
    uint16_t net_n;
    pop_attribute(buffer_ptr, &net_n, attrib_size);

    // Transforms network long to host long
    *n = ntohs(net_n);
}


void serialize_message(const Message *message, uint8_t *buffer, size_t *buffer_size)
{
    uint8_t* buffer_ptr = buffer;

    // Serializes message type
    push_attribute(&buffer_ptr, &message->base.type, sizeof(message->base.type));
    push_long(&buffer_ptr, &message->base.payload_length, sizeof(message->base.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->base.type)
    {
    case MESSAGE_TYPE_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        push_attribute(&buffer_ptr, chat_message->user_base.username, MAX_USERNAME_BYTES);
        push_attribute(&buffer_ptr, chat_message->text, MAX_CHAT_TEXT_BYTES);
    }
        break;
    
    case MESSAGE_TYPE_FILE_INFO:
    {
        FileInfoMsg *file_message = (FileInfoMsg*)message;
        push_attribute(&buffer_ptr, file_message->name, sizeof(file_message->name));
        push_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
    }
        break;

    
    default:
        break;
    }

    // Buffer size is calculated as the difference between the pointers
    *buffer_size = buffer_ptr - buffer;
}

void deserialize_message(const uint8_t* buffer, Message *message)
{
    uint8_t* buffer_ptr = (uint8_t*)buffer;

    // Pops type
    pop_attribute(&buffer_ptr, &message->base.type, sizeof(message->base.type));

    // Pops payload length
    pop_long(&buffer_ptr, &message->base.payload_length, sizeof(message->base.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->base.type)
    {
    case MESSAGE_TYPE_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        pop_attribute(&buffer_ptr, chat_message->user_base.username, MAX_USERNAME_BYTES);
        pop_attribute(&buffer_ptr, chat_message->text, MAX_CHAT_TEXT_BYTES);
    }
        break;

    case MESSAGE_TYPE_FILE_INFO:
    {
        FileInfoMsg *file_message = (FileInfoMsg*)message;
        pop_attribute(&buffer_ptr, file_message->name, sizeof(file_message->name));
        pop_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
    }
        break;

    default:
        break;
    }
}