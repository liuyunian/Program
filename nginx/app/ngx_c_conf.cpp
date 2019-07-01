#include <stdio.h>


#include "ngx_c_conf.h"
#include "ngx_global.h"

ConfFileProcessor * ConfFileProcessor::instance = nullptr;

ConfFileProcessor::ConfFileProcessor(const std::string & confFileName) : 
    m_confFileName(confFileName){}

ConfFileProcessor::~ConfFileProcessor(){

}

bool ConfFileProcessor::load(){
    FILE * fp;
    fp = fopen(m_confFileName.c_str(), "r");
    if(!fp){
        return false;
    }

    char lineBuf[CONF_LINE_SIZE];
}
