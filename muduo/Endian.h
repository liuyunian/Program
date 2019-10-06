#ifndef ENDIAN_H_
#define ENDIAN_H_

#include <stdint.h>

#ifdef __linux__
#include <endian.h>
#else
#include <netinet/in.h>
#endif

namespace sockets { 

inline uint64_t host_to_network64(uint64_t host64){
#ifdef __linux__
    return htobe64(host64);
#else
    uint32_t high = static_cast<uint32_t>(host64 / 0x100000000);
    uint32_t low = static_cast<uint32_t>(host64 % 0x100000000);
    return static_cast<uint64_t>(htonl(low)) * 0x100000000 + htonl(high);
#endif
}

inline uint32_t host_to_network32(uint32_t host32){
#ifdef __linux__
    return htobe32(host32);
#else
    return htonl(host32);
#endif
}

inline uint16_t host_to_network16(uint16_t host16){
#ifdef __linux__
    return htobe16(host16);
#else
    return htons(host16);
#endif
}

inline uint64_t network_to_host64(uint64_t net64){
#ifdef __linux__
    return be64toh(net64);
#else
    uint32_t high = static_cast<uint32_t>(net64 / 0x100000000);
    uint32_t low = static_cast<uint32_t>(net64 % 0x100000000);
    return static_cast<u_int64_t>(ntohl(low)) * 0x100000000 + ntohl(high);
#endif
}

inline uint32_t network_to_host32(uint32_t net32){
#ifdef __linux__
    return be32toh(net32);
#else
    return ntohl(net32);
#endif
}

inline uint16_t network_to_host16(uint16_t net16){
#ifdef __linux__
    return be16toh(net16);
#else
    return ntohs(net16);
#endif
}

} // namespace sockets

#endif // ENDIAN_H_