/**
 * 放置一些与字符串处理相关的函数
 */

#include <string.h>


/**
 * 截取字符串尾部的空格
 */
void Rtrim(char * string){ 
	if(string == NULL)   
		return;   

	size_t len = strlen(string);   
	while(len > 0 && string[len-1] == ' '){
        string[--len] = 0; 
    }
}

/**
 * 截取字符首部的空格
 */
void Ltrim(char * string){
	size_t len = strlen(string);   
	char * p_tmp = string;
	if((*p_tmp) != ' ') //不是以空格开头
		return;

	while((*p_tmp) != '\0')
	{
		if((*p_tmp) == ' ')
			p_tmp++;
		else
			break;
	}

	if((*p_tmp) == '\0'){ // 字符串string全是空格
		*string = '\0';
		return;
	}

	while((*p_tmp) != '\0')
	{
		(*string) = (*p_tmp);
		p_tmp++;
		string++;
	}
	*string = '\0';
}