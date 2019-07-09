#ifndef NGX_FUNC_H__
#define NGX_FUNC_H__

/**
 * 字符串相关函数
*/
void Rtrim(char *string);
void Ltrim(char *string);

/**
 * 设置标题相关函数
 */
void moveEnviron();
void setTitle(const char * title);
void freeEnviron();

#endif  