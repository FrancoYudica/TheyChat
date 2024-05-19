#ifndef __NET_STREAM_H__
#define __NET_STREAM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "message.h"


/// @brief Structure to manage a network stream buffer.
typedef struct
{
    /// @brief Buffer where unprocessed bytes are stored.
    uint8_t stream[4096];

    /// @brief Amount of unprocessed bytes.
    uint32_t written_bytes;
} NetworkStream;


/// @brief Initializes the network stream.
///
/// This function sets up a NetworkStream by clearing its buffer and resetting
/// the count of written bytes.
///
/// @param ns Pointer to the NetworkStream to initialize.
void init_net_stream(NetworkStream *ns);

/// @brief Reads network stream and pops a message if possible.
///
/// This function attempts to parse a complete message from the network stream.
/// If a complete message is found, it is removed from the stream and returned.
/// If not, the function returns NULL.
///
/// @param ns Pointer to the NetworkStream to read from.
/// @return Pointer to the parsed Message if a complete message is found, NULL otherwise.
Message* stream_pop_message(NetworkStream *ns);

/// @brief Reads network stream and pops a message of the specified type if possible.
///
/// This function attempts to parse a complete message of the specified type from the network stream.
/// If a complete message of the expected type is found, it is removed from the stream and returned.
/// If no message is found, the function returns NULL. If the message type does not match the expected type,
/// an exception is thrown.
///
/// @param expected_type The expected type of the message.
/// @param ns Pointer to the NetworkStream to read from.
/// @return Pointer to the parsed Message if a complete message of the expected type is found, NULL otherwise.
/// @throws SomeException If the message type does not match the expected type.
Message* stream_pop_message_by_type(uint8_t expected_type, NetworkStream* ns);

#endif