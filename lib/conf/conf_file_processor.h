#ifndef CONF_FILE_PROCESSOR_H_
#define CONF_FILE_PROCESSOR_H_

#include <string>
#include <map>

#define CONF_LINE_SZ 500
#define CONF_NAME_SZ 50
#define CONF_CONTENT_SZ 400

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
    bool load(const char * confFileName);

    /**
     * @brief 根据配置项的名字获取字符串类型的内容
     * @param itemName 配置项名字
     * @return 如果该配置项存在那么返回配置项内容，否则返回nullptr
     */
    const char * getItemContent_str(const std::string & itemName);

    /**
     * @brief 根据配置项的名字获取整数类型的内容
     * @param itemName 配置项名字
     * @return 如果该配置项存在那么返回配置项内容，否则返回-1
     */
    int getItemContent_int(const std::string & itemName);

    /**
     * @brief 根据配置项的名字获取整数类型的内容，并可以指定默认值
     * @param itemName 配置项名字
     * @param def 默认值
     * @return 如果该配置项存在那么返回配置项内容，否则返回指定的默认值
     */
    int getItemContent_int(const std::string & itemName, const int def);

private:
    /**
     * @brief 截取字符串尾部的空格
     */
    void trimTailSpace(char * string);

    /**
     * @brief 截取字符首部的空格
     */
    void trimHeadSpace(char * string);

private:
    std::map<std::string, std::string> m_confItemStore;
};

#endif // CONF_FILE_PROCESSOR_H_