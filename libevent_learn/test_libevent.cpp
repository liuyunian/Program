#include <iostream>
#include <event2/event.h>

int main(){
#ifdef _WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

	event_base * base = event_base_new();
	if(base){
		std::cout << "event_base_new()执行成功！" << std::endl;
	}

    return 0;
}
