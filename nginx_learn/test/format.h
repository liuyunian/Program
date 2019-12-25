#ifndef FORMAT_H_
#define FORMAT_H_

#include <stdint.h>

/**
 * @brief 包头
 * 客户端和服务器端协商的数据包格式：包头+包体
 */
#pragma pack(1) // 采用1字节对齐

struct PktHeader{
    uint16_t len; // 记录数据包的长度
    uint16_t type; // 类型
    int crc32; // 用于CRC32校验
    // ... 待扩展
};

#pragma pack()

#endif // FORMAT_H_