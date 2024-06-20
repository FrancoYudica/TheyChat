#include "net/serialization/net_serializer.h"
#include "arpa/inet.h"
#include <string.h>

void ns_push_byte_array(uint8_t** dest, const uint8_t* src, size_t src_size)
{
    // Copy src to the location pointed by *dest
    memcpy(*dest, src, src_size);
    // Advance the dest pointer by the size of the src
    *dest += src_size;
}

void ns_push_long(uint8_t** dest, const uint32_t* n)
{
    // Transform host long to network long
    uint32_t net_n = htonl(*n);
    // Copy the converted value into the buffer
    ns_push_byte_array(dest, (uint8_t*)&net_n, sizeof(uint32_t));
}

void ns_push_short(uint8_t** dest, const uint16_t* n)
{
    // Transform host short to network short
    uint16_t net_n = htons(*n);
    // Copy the converted value into the buffer
    ns_push_byte_array(dest, (uint8_t*)&net_n, sizeof(uint16_t));
}

// Push uint64_t
void ns_push_long_long(uint8_t** dest, const uint64_t* n)
{
    uint64_t net_n = htobe64(*n); // host to big-endian (network byte order)
    ns_push_byte_array(dest, (uint8_t*)&net_n, sizeof(uint64_t));
}

void ns_pop_byte_array(uint8_t** src, uint8_t* dest, size_t dest_size)
{
    // Copy from *src to dest
    memcpy(dest, *src, dest_size);
    // Advance the src pointer by the size of the dest
    *src += dest_size;
}

void ns_pop_long(uint8_t** src, uint32_t* n)
{
    uint32_t net_n;
    // Copy the value from the buffer
    ns_pop_byte_array(src, (uint8_t*)&net_n, sizeof(uint32_t));
    // Convert from network to host byte order
    *n = ntohl(net_n);
}

void ns_pop_short(uint8_t** src, uint16_t* n)
{
    uint16_t net_n;
    // Copy the value from the buffer
    ns_pop_byte_array(src, (uint8_t*)&net_n, sizeof(uint16_t));
    // Convert from network to host byte order
    *n = ntohs(net_n);
}

// Pop uint64_t
void ns_pop_long_long(uint8_t** src, uint64_t* n)
{
    uint64_t net_n;
    ns_pop_byte_array(src, (uint8_t*)&net_n, sizeof(uint64_t));
    *n = be64toh(net_n); // big-endian (network byte order) to host
}