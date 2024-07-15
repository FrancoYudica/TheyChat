#ifndef __NET_SERIALIZER_H__
#define __NET_SERIALIZER_H__

/**
 * @file net_serializer.h
 * @brief Functions for serializing and deserializing data to/from network byte order.
 */

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Copies a byte array into a buffer and updates the pointer.
 *
 * @param dest Pointer to the buffer pointer to be updated.
 * @param src Source byte array to copy.
 * @param src_size Size of the source byte array.
 */
void ns_push_byte_array(uint8_t** dest, const uint8_t* src, size_t src_size);
/**
 * @brief Converts uint16_t to network byte order and copies it to a buffer.
 *
 * @param dest Pointer to the buffer pointer to be updated.
 * @param n Pointer to the uint16_t value.
 */
void ns_push_16(uint8_t** dest, const uint16_t* n);
/**
 * @brief Converts uint32_t to network byte order and copies it to a buffer.
 *
 * @param dest Pointer to the buffer pointer to be updated.
 * @param n Pointer to the uint32_t value.
 */
void ns_push_32(uint8_t** dest, const uint32_t* n);

/**
 * @brief Converts uint64_t to network byte order and copies it to a buffer.
 *
 * @param dest Pointer to the buffer pointer to be updated.
 * @param n Pointer to the uint64_t value.
 */
void ns_push_64(uint8_t** dest, const uint64_t* n);

/**
 * @brief Extracts a byte array from a buffer and updates the pointer.
 *
 * @param src Pointer to the buffer pointer to be updated.
 * @param dest Destination byte array.
 * @param dest_size Size of the destination byte array.
 */
void ns_pop_byte_array(uint8_t** src, uint8_t* dest, size_t dest_size);

/**
 * @brief Extracts a uint16_t from the buffer, converting from network to host byte order.
 *
 * @param src Pointer to the buffer pointer to be updated.
 * @param n Pointer to the uint16_t value.
 */
void ns_pop_16(uint8_t** src, uint16_t* n);

/**
 * @brief Extracts a uint32_t from the buffer, converting from network to host byte order.
 *
 * @param src Pointer to the buffer pointer to be updated.
 * @param n Pointer to the uint32_t value.
 */
void ns_pop_32(uint8_t** src, uint32_t* n);

/**
 * @brief Extracts a uint64_t from the buffer, converting from network to host byte order.
 *
 * @param src Pointer to the buffer pointer to be updated.
 * @param n Pointer to the uint64_t value.
 */
void ns_pop_64(uint8_t** src, uint64_t* n);

#endif
