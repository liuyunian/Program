#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_include/ngx_global.h"

extern char ** environ;
char * new_environ = NULL;

void ngx_move_environ(){
    // 统计环境变量所占用的内存
    size_t len = 0;
    for(int i = 0; environ[i]; ++ i){
        len += strlen(environ[i]) + 1;
    }

    // 创建新的存储空间，用于存放环境变量
    new_environ = new char[len];
    memset(new_environ, 0, len);

    // 将环境变量复制到新创建的空间
    char * old_env = environ[0];

    char * tmp_env = new_environ;
    for(int i = 0; environ[i]; ++ i){
        size_t size = strlen(environ[i]) + 1;
        strcpy(tmp_env, environ[i]);
        environ[i] = tmp_env;
        tmp_env += size;
    }

    // 将原来存储环境变量的空间置0
    memset(old_env, 0, len);
}

void ngx_set_title(const char * title){
    // 假设命令行参数都不再使用了
    size_t len = 0;
    for(int i = 0; g_argv[i]; ++ i){
        len += strlen(g_argv[i]) + 1;
    }
    memset(g_argv[0], 0, len);
    g_argv[1] = NULL;

    // 拷贝标题到argv[0]
    size_t title_len = strlen(title);
    char * tmp_argv = g_argv[0];
    strcpy(tmp_argv, title);
    tmp_argv += title_len;
}

void ngx_free_environ(){
    if(new_environ){
        delete[] new_environ;
        new_environ = NULL;
    }
}