#include <stdio.h>
#include <string.h>

#include <string>

#include "conf_file_processor.h"

ConfFileProcessor * ConfFileProcessor::instance = nullptr;

ConfFileProcessor::ConfFileProcessor(){}

ConfFileProcessor::~ConfFileProcessor(){}

bool ConfFileProcessor::load(const char * confFileName){
    FILE * fp;
    fp = fopen(confFileName, "r");
    if(!fp){
        return false;
    }

    char lineBuf[CONF_LINE_SZ];
    while(!feof(fp)){
        if(fgets(lineBuf, CONF_LINE_SZ, fp) == nullptr){
            continue;
        }

        if(strlen(lineBuf) == 0){
            continue;
        }

        // 这里的空格表示配置信息必须要顶格写，不能有空格（保证了itemName左边不可能有空格）
        if(lineBuf[0] == '#' || lineBuf[0] == '[' || lineBuf[0] == ' ' || lineBuf[0] == '\t' || lineBuf[0] == '\n' || lineBuf[0] == ';'){
            continue;
        }

procString:
        size_t len = strlen(lineBuf);
        if(lineBuf[len-1] == '\r' || lineBuf[len-1] == '\n' || lineBuf[len-1] == ' '){ // 这里保证了itemContent右侧不可能有空格
            lineBuf[len-1] = 0;
            goto procString;
        }

        char * pos = strchr(lineBuf, '=');
        if(pos != nullptr){
            char itemName[CONF_NAME_SZ] = {0};
            char itemContent[CONF_CONTENT_SZ] = {0};
            strncpy(itemName, lineBuf, (size_t)(pos-lineBuf));
            strcpy(itemContent, pos + 1);

            trimTailSpace(itemName);
			trimHeadSpace(itemContent);

            m_confItemStore.insert({std::string(itemName), std::string(itemContent)});
        }
    }

    fclose(fp);
    return true;
}

const char * ConfFileProcessor::getItemContent_str(const std::string & itemName){
    auto iter = m_confItemStore.find(itemName);
    if(iter != m_confItemStore.end()){
        return iter->second.c_str();
    }

    return nullptr;
}

int ConfFileProcessor::getItemContent_int(const std::string & itemName){
    auto iter = m_confItemStore.find(itemName);
    if(iter != m_confItemStore.end()){
        return std::stoi(iter->second);
    }

    return -1;
}

int ConfFileProcessor::getItemContent_int(const std::string & itemName, const int def){
    auto iter = m_confItemStore.find(itemName);
    if(iter != m_confItemStore.end()){
        return std::stoi(iter->second);
    }

    return def;
}

void ConfFileProcessor::trimTailSpace(char * string){ 
	if(string == nullptr)   
		return;   

	size_t len = strlen(string);   
	while(len > 0 && string[len-1] == ' '){
        string[len-1] = 0;
		-- len;
    }
}

void ConfFileProcessor::trimHeadSpace(char * string){
	size_t len = strlen(string);
	char * p_tmp = string;
	if((*p_tmp) != ' ') //不是以空格开头
		return;

	while((*p_tmp) != '\0'){
		if((*p_tmp) == ' ')
			++ p_tmp;
		else
			break;
	}

	if((*p_tmp) == '\0'){ // 字符串string全是空格
		*string = '\0';
		return;
	}

	while((*p_tmp) != '\0'){
		(*string) = (*p_tmp);
		++ p_tmp;
		++ string;
	}
	*string = '\0';
}
