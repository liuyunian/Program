#ifndef NGX_C_CONF_H_
#define NGX_C_CONF_H_

#include <string>

class ConfFileProcessor{
private:
    ConfFileProcessor(const std::string & confFileName);

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
    static ConfFileProcessor * getInstance(const std::string & confFileName){
        if(instance == nullptr){
            instance = new ConfFileProcessor(confFileName);
            static GCInstance gc;
        }

        return instance;
    }

    bool load();

private:
    std::string m_confFileName;
    
};

#endif