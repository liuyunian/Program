#ifndef ERROR_H_
#define ERROR_H_

extern void err_ret(const char * , ...);
extern void err_sys(const char * , ...);
extern void err_dump(const char * , ...);
extern void err_msg(const char * , ...);
extern void err_quit(const char * , ...);

#endif