#ifndef NGX_C_CONF_H_
#define NGX_C_CONF_H_

#include <string>
#include <map>

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

    /**
     * @brief 读取配置文件内容
     * @param confFileName配置文件名（绝对路径或者相对路径）
     * @return 成功返回true，否则返回false
     */
    bool ngx_conf_load(const char * confFileName);

    /**
     * @brief 根据配置项的名字获取字符串类型的内容
     * @param itemName 配置项名字
     * @return 如果该配置项存在那么返回配置项内容，否则返回nullptr
     */
    const char * ngx_conf_getContent_str(const std::string & itemName);

    /**
     * @brief 根据配置项的名字获取整数类型的内容
     * @param itemName 配置项名字
     * @return 如果该配置项存在那么返回配置项内容，否则返回-1
     */
    int ngx_conf_getContent_int(const std::string & itemName);

    /**
     * @brief 根据配置项的名字获取整数类型的内容，并可以指定默认值
     * @param itemName 配置项名字
     * @param def 默认值
     * @return 如果该配置项存在那么返回配置项内容，否则返回指定的默认值
     */
    int ngx_conf_getContent_int(const std::string & itemName, const int def);

private:
    /**
     * @brief 截取字符串尾部的空格
     */
    void ngx_string_trim_tailSpace(char * string);

    /**
     * @brief 截取字符首部的空格
     */
    void ngx_string_trim_headSpace(char * string);

private:
    std::map<std::string, std::string> m_confItemStore;
};

#endif