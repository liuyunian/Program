#ifndef NGX_GLOBAL_H_
#define NGX_GLOBAL_H_

/**
 * 配置文件
 */
#define CONF_LINE_SIZE 500
#define CONF_NAME_SIZE 50
#define CONF_CONTENT_SIZE 400

struct CConfItem{
	char itemName[CONF_NAME_SIZE];
	char itemContent[CONF_CONTENT_SIZE];
};

/**
 * 设置标题
 */
extern char ** g_argv;

#endif