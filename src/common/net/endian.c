#include "net/endian.h"
#include "net/socket.h"

uint16_t ntoh16(uint16_t net_16bits)
{
    return ntohs(net_16bits);
}
uint16_t hton16(uint16_t host_16bits)
{
    return htons(host_16bits);
}

uint32_t ntoh32(uint32_t net_32bits)
{
    return ntohl(net_32bits);
}
uint32_t hton32(uint32_t host_32bits)
{
    return htonl(host_32bits);
}

uint64_t ntoh64(uint64_t net_64bits)
{
#if defined(_WIN32) || defined(_WIN64)
    // Windows doesn't provide be64toh or htobe64, so we need to implement it
    uint64_t result = 0;
    uint8_t* p = (uint8_t*)&result;
    p[0] = (net_64bits >> 56) & 0xFF;
    p[1] = (net_64bits >> 48) & 0xFF;
    p[2] = (net_64bits >> 40) & 0xFF;
    p[3] = (net_64bits >> 32) & 0xFF;
    p[4] = (net_64bits >> 24) & 0xFF;
    p[5] = (net_64bits >> 16) & 0xFF;
    p[6] = (net_64bits >> 8) & 0xFF;
    p[7] = net_64bits & 0xFF;
    return result;
#else
    // On Unix-like systems, use the provided function
    return be64toh(net_64bits);
#endif
}

uint64_t hton64(uint64_t host_64bits)
{
#if defined(_WIN32) || defined(_WIN64)
    // Windows doesn't provide be64toh or htobe64, so we need to implement it
    uint64_t result = 0;
    uint8_t* p = (uint8_t*)&host_64bits;
    result = ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) | ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) | ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) | ((uint64_t)p[6] << 8) | ((uint64_t)p[7]);
    return result;
#else
    // On Unix-like systems, use the provided function
    return htobe64(host_64bits);
#endif
}