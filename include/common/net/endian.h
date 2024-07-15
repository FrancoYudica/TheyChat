#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <stdint.h>

// Converts a 16-bit integer from network byte order to host byte order
uint16_t ntoh16(uint16_t net_16bits);

// Converts a 16-bit integer from host byte order to network byte order
uint16_t hton16(uint16_t host_16bits);


// Converts a 32-bit integer from network byte order to host byte order
uint32_t ntoh32(uint32_t net_32bits);

// Converts a 32-bit integer from host byte order to network byte order
uint32_t hton32(uint32_t host_32bits);


// Converts a 64-bit integer from network byte order to host byte order
uint64_t ntoh64(uint64_t net_64bits);

// Converts a 64-bit integer from host byte order to network byte order
uint64_t hton64(uint64_t host_64bits);

#endif