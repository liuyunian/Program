#ifndef NETFUNC_H_
#define NETFUNC_H_

#include <stdint.h>

void getMacAddress(const char * interface, uint8_t * macAddr);

char * getIPv4Address(const char * interface);

#endif // NETFUNC_H_