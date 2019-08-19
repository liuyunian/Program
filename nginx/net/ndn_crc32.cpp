/**
 * CRC校验算法函数
 * 感觉没有必要封装成类
*/
#include <stdint.h>

static uint32_t crc32_table[256];

static int ngx_crc32_reflect(uint32_t ref, char ch);

void ngx_crc32_table_init(){
    // This is the official polynomial used by CRC-32 in PKZip, WinZip and Ethernet. 
    uint32_t ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; ++ i)
	{
		crc32_table[i]=Reflect(i, 8) << 24;
        
		for(int j = 0; j < 8; j++){
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
        }

		crc32_table[i] = Reflect(crc32_table[i], 32);
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

//用crc32_table寻找表来产生数据的CRC值
int ngx_crc32_get_value(unsigned char * buffer, uint32_t dwSize){
	// Be sure to use unsigned variables,
	// because negative values introduce high bits
	// where zero bits are required.
    uint32_t crc(0xffffffff);
	int len;
	
	len = dwSize;	
	// Perform the algorithm on each character
	// in the string, using the lookup table values.
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];
	// Exclusive OR the result with the beginning value.
	return crc^0xffffffff;
}