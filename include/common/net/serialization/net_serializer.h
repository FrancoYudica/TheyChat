#ifndef __NET_SERIALIZER_H__
#define __NET_SERIALIZER_H__

/**
 * @file net_serializer.h
 * @brief Functions for serializing and deserializing data to/from network byte order.
 */

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Copies a byte array into a destination buffer and updates the destination pointer.
 *
 * @param dest Pointer to the destination buffer pointer. This will be updated to the new position after copying.
 * @param src Source byte array to copy.
 * @param src_size Size of the source byte array.
 */
void ns_push_byte_array(uint8_t** dest, const uint8_t* src, size_t src_size);

/**
 * @brief Converts a uint32_t value from host to network byte order and copies it into the destination buffer.
 *
 * @param dest Pointer to the destination buffer pointer. This will be updated to the new position after copying.
 * @param n Pointer to the uint32_t value to convert and copy.
 * @param src_size Size of the source byte array (should be sizeof(uint32_t)).
 */
void ns_push_long(uint8_t** dest, const uint32_t* n, size_t src_size);

/**
 * @brief Converts a uint16_t value from host to network byte order and copies it into the destination buffer.
 *
 * @param dest Pointer to the destination buffer pointer. This will be updated to the new position after copying.
 * @param n Pointer to the uint16_t value to convert and copy.
 * @param src_size Size of the source byte array (should be sizeof(uint16_t)).
 */
void ns_push_short(uint8_t** dest, const uint16_t* n, size_t src_size);

/**
 * @brief Extracts a byte array from a source buffer and updates the source pointer.
 *
 * @param src Pointer to the source buffer pointer. This will be updated to the new position after copying.
 * @param dest Destination byte array where the data will be copied.
 * @param dest_size Size of the destination byte array.
 */
void ns_pop_byte_array(uint8_t** src, uint8_t* dest, size_t dest_size);

/**
 * @brief Extracts a uint32_t value from the source buffer, converting it from network to host byte order.
 *
 * @param src Pointer to the source buffer pointer. This will be updated to the new position after copying.
 * @param n Pointer to the uint32_t value to store the converted result.
 * @param src_size Size of the source byte array (should be sizeof(uint32_t)).
 */
void ns_pop_long(uint8_t** src, uint32_t* n, size_t src_size);

/**
 * @brief Extracts a uint16_t value from the source buffer, converting it from network to host byte order.
 *
 * @param src Pointer to the source buffer pointer. This will be updated to the new position after copying.
 * @param n Pointer to the uint16_t value to store the converted result.
 * @param src_size Size of the source byte array (should be sizeof(uint16_t)).
 */
void ns_pop_short(uint8_t** src, uint16_t* n, size_t src_size);

#endif