#include "net_serializer.h"
#include "arpa/inet.h"


void ns_push_byte_array(uint8_t **dest, const uint8_t *src, size_t src_size) 
{
    // Copy src to the location pointed by *dest
    memcpy(*dest, src, src_size);
    // Advance the dest pointer by the size of the src
    *dest += src_size;
}


void ns_push_long(uint8_t **dest, const uint32_t *n, size_t src_size)
{
    // Transform host long to network long
    uint32_t net_n = htonl(*n);
    // Copy the converted value into the buffer
    ns_push_byte_array(dest, (uint8_t *)&net_n, src_size);
}


void ns_push_short(uint8_t **dest, const uint16_t *n, size_t src_size) 
{
    // Transform host short to network short
    uint16_t net_n = htons(*n);
    // Copy the converted value into the buffer
    ns_push_byte_array(dest, (uint8_t *)&net_n, src_size);
}


void ns_pop_byte_array(uint8_t **src, uint8_t *dest, size_t dest_size) 
{
    // Copy from *src to dest
    memcpy(dest, *src, dest_size);
    // Advance the src pointer by the size of the dest
    *src += dest_size;
}


void ns_pop_long(uint8_t **src, uint32_t *n, size_t src_size) 
{
    uint32_t net_n;
    // Copy the value from the buffer
    ns_pop_byte_array(src, (uint8_t *)&net_n, src_size);
    // Convert from network to host byte order
    *n = ntohl(net_n);
}


void ns_pop_short(uint8_t **src, uint16_t *n, size_t src_size) 
{
    uint16_t net_n;
    // Copy the value from the buffer
    ns_pop_byte_array(src, (uint8_t *)&net_n, src_size);
    // Convert from network to host byte order
    *n = ntohs(net_n);
}