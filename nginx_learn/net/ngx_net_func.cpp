/**
 * 该源文件放置通用的网络函数
 */

#include <stdint.h>
#include <sys/ioctl.h>

static uint32_t crc32_table[256]; // CRC32校验查找表

static int ngx_crc32_reflect(uint32_t ref, char ch);

/**
 * @brief 将socket描述符设置为非阻塞模式
 * @param sockfd要设置的描述符
 * @return 成功执行返回0，否则返回-1
 */
int ngx_set_nonblocking(int sockfd){
    int nb = 1; // 0：清除，1：设置  
    if(ioctl(sockfd, FIONBIO, &nb) < 0){ //FIONBIO：设置/清除非阻塞I/O标记：0：清除，1：设置
        return -1;
    }

    return 0;
}

/**
 * @brief 初始化CRC32校验查找表
*/
void ngx_crc32_table_init(){
    // This is the official polynomial used by CRC-32 in PKZip, WinZip and Ethernet. 
    uint32_t ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; ++ i){
		crc32_table[i]=ngx_crc32_reflect(i, 8) << 24;
        
		for(int j = 0; j < 8; j++){
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
        }

		crc32_table[i] = ngx_crc32_reflect(crc32_table[i], 32);
	}
}

/**
 * @brief 获取指定数据对应的CRC32校验值
 * @param buffer要计算CRC32校验值的数据
 * @param bufSZ数据大小
 * @return CRC32校验值
*/

int ngx_crc32_get_value(uint8_t * buffer, uint32_t bufSZ){
	// Be sure to use unsigned variables,
	// because negative values introduce high bits
	// where zero bits are required.
    uint32_t crc(0xffffffff);
	int len = bufSZ;
	// Perform the algorithm on each character
	// in the string, using the lookup table values.
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];
	// Exclusive OR the result with the beginning value.
	return crc^0xffffffff;
}

int ngx_crc32_reflect(uint32_t ref, char ch){
    // Used only by Init_CRC32_Table()
    uint32_t value(0);

	// Swap bit 0 for bit 7 , bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); ++ i){
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}

	return value;
}
