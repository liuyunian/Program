#include <iostream>

#include "conf_file_processor.h"

int main(){
    // 加载配置文件
    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    if(!confProcessor->load("nginx.conf")){
        std::cerr << "Failed to load nginx.conf" << std::endl;
        exit(1);
    }

    std::cout << confProcessor->getItemContent_str("LogFile") << std::endl;
    std::cout << confProcessor->getItemContent_int("LogLevel") << std::endl;
    std::cout << confProcessor->getItemContent_int("Log", 5) << std::endl;

    return 0;
}