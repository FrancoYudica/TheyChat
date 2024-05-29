#ifndef __MESSAGE_SERIALIZER_H__
#define __MESSAGE_SERIALIZER_H__
#include <stddef.h>
#include "messages/message.h"

/**
 * @brief Serializes a Message structure into a buffer.
 *
 * This function serializes the provided Message structure into a buffer.
 * It converts the fields of the Message structure to network byte order
 * using appropriate functions (e.g., htonl) and copies them into the buffer.
 * The buffer size is updated to reflect the size of the serialized data.
 *
 * @param message Pointer to the Message structure to serialize.
 * @param buffer Pointer to the buffer where the serialized data will be stored.
 * @param buffer_size Pointer to the variable storing the size of the buffer.
 *                   On output, it will be updated to reflect the size of the serialized data.
 * @return void
 */
void ns_serialize_message(const Message* message, uint8_t* buffer, size_t* buffer_size);

/**
 * @brief Deserializes a buffer into a Message structure.
 *
 * This function deserializes data from the provided buffer into a Message structure.
 * It converts the fields of the buffer from network byte order to host byte order
 * using appropriate functions (e.g., ntohl) and copies them into the Message structure.
 *
 * @param buffer Pointer to the buffer containing the serialized data.
 * @param message Pointer to the Message structure where the deserialized data will be stored.
 * @return void
 */
void ns_deserialize_message(const uint8_t* buffer, Message* message);

#endif