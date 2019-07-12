#ifndef NGX_C_CONF_H_
#define NGX_C_CONF_H_

#include <string>
#include <vector>

#include "ngx_global.h"

class ConfFileProcessor{
private:
    ConfFileProcessor();

    ~ConfFileProcessor(); 

    static ConfFileProcessor * instance;

    class GCInstance{
    public:
        ~GCInstance(){
            if(ConfFileProcessor::instance != nullptr){
                delete ConfFileProcessor::instance;
                ConfFileProcessor::instance = nullptr;
            }
        }
    };

public:
    static ConfFileProcessor * getInstance(){
        if(instance == nullptr){
            instance = new ConfFileProcessor;
            static GCInstance gc;
        }

        return instance;
    }

    bool load(const char * confFileName);

    const char * getItemContent_str(const char * itemName);

    int getItemContent_int(const char * itemName, const int def);

private:
    std::vector<ConfItem *> m_confItemList;
};

#endif