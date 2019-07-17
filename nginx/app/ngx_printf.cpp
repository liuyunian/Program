#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "ngx_macro.h"
#include "ngx_func.h"

static u_char * ngx_sprintf_num(u_char *, u_char *, uint64_t, u_char, int, uint32_t);

u_char * ngx_slprintf(u_char * buf, u_char * last, const char * fmt, ...){
    va_list ap;

    va_start(ap, fmt);
    buf = ngx_vslprintf(buf, last, fmt, ap);
    va_end(ap);

    return buf;
}

u_char * ngx_vslprintf(u_char * buf, u_char * last, const char * fmt, va_list ap){
    int64_t i64; // 保存有符号整数
    uint64_t ui64; // 保存无符号整数
    char ch; // 保存字符
    char * string; // 保存字符串
    double fn; // 保存浮点数
    u_int64_t frac; //保存小数部分

    u_char padding;
    int hex; //是否以16进制形式显示，0：不是，1：是，并以小写字母显示a-f，2：是，并以大写字母显示A-F
    u_int32_t width; // 用于存放%5d, %7f这种%后面的数字
    u_int32_t frac_width; // 用于保存%7.2这种小数点后的位数

    while(*fmt && buf < last){
        if(*fmt == '%'){
            // 初始化
            i64 = 0;
            ui64 = 0;
            ch = 0;
            string = NULL;
            fn = 0;
            frac = 0;

            hex = 0;
            width = 0;
            frac_width = 6; // 默认是显示小数点后6位，如果实际的数字不够，则填充0

            ++ fmt;
            padding = (*fmt == '0' ? '0' : ' ');

            while(*fmt >= '0' && *fmt <= '9'){
                width = width * 10 + (*fmt - '0');
                ++ fmt;
            }

            while(1){
                switch(*fmt){
                    case 'x': {
                        hex = 1;
                        ++ fmt;
                        continue;
                    }

                    case 'X': {
                        hex = 2;
                        ++ fmt;
                        continue;
                    }

                    case '.': {
                        ++ fmt;
                        frac_width = 0;
                        while(*fmt >= 0 && *fmt <= '9'){
                            frac_width = frac_width * 10 + (*fmt - '0');
                            ++ fmt;
                        }
                        continue;
                    }
                    
                    default: {
                        break; // leave switch
                    }
                }

                break; // leave while
            }

            switch(*fmt){
                case '%': {
                    *buf = '%';
                    ++ buf;
                    ++ fmt;
                    continue; //针对while循环，对switch没有作用
                }

                case 'd': {
                    i64 = (int64_t)va_arg(ap, int);
                    if(i64 < 0){
                        *buf = '-';
                        ++ buf;

                        ui64 = (u_int64_t)(-i64);
                    }
                    else{
                        ui64 = (u_int64_t)i64;
                    }
                    buf = ngx_sprintf_num(buf, last, ui64, padding, hex, width);

                    ++ fmt;
                    continue;
                }

                case 'u': {
                    ui64 = va_arg(ap, u_int64_t);
                    buf = ngx_sprintf_num(buf, last, ui64, padding, hex, width);

                    ++ fmt;
                    continue;
                }

                case 'c': {
                    ch = (char)va_arg(ap, int);
                    *buf = ch;
                    ++ buf;

                    ++ fmt;
                    continue;
                }

                case 's':{
                    string = va_arg(ap, char *);
                    size_t string_len = strlen(string);
                    if(buf + string_len >= last){
                        string_len = last - buf;
                    }
                    buf = ngx_memcpy(buf, string, string_len);

                    ++ fmt;
                    continue;
                }

                case 'f': {
                    fn = va_arg(ap, double);
                    if(fn < 0){
                        *buf = '-';
                        ++ buf;
                        fn = -fn;
                    }
                    ui64 = (u_int64_t)fn; // 保存浮点数的整数部分
                    frac = (fn - ui64) * pow(10, frac_width) + 0.5; // 保留浮点数的小数部分
                    if(frac == pow(10, frac_width)){
                        ++ ui64;
                        frac = 0;
                    }

                    buf = ngx_sprintf_num(buf, last, ui64, padding, 0, width); // 将整数部分放入buf
                    if(buf < last){
                        *buf = '.';
                        ++ buf;
                    }
                    buf = ngx_sprintf_num(buf, last, frac, '0', 0, frac_width); // 将小数部分放入buf

                    ++ fmt;
                    continue;
                }
                /*
                    后面逐步增加的格式
                    case 'e' 以指数形式输出单、双精度实数
                    case 'g' 以%f%e中较短的输出宽度输出单、双精度实数
                    case 'o' 以八进制形式输出无符号整数(不输出前缀O) 
                    case 'p' 输出地址
                 */

                default: {
                    *buf = *fmt;
                    ++ buf;
                    ++ fmt;
                }
            }
        }
        else{
            *buf = *fmt;
            ++ buf;
            ++ fmt;
        }
    }

    return buf;
}

static u_char * ngx_sprintf_num(u_char * buf, u_char * last, uint64_t ui64, u_char padding, int hexFlag, uint32_t width){
    u_char temp[NGX_INT64_LEN + 1]; // temp[0...20]
    memset(temp, 0, NGX_INT64_LEN + 1); // 置零
    u_char * tmp_last = temp + NGX_INT64_LEN; // NGX_INT64_LEN = 20，所以p指向的是temp[20]那个位置，也就是数组最后一个元素位置，这个位置必须是'\0' 

    static u_char hex[] = "0123456789abcdef";
    static u_char HEX[] = "0123456789ABCDEF";

    if(hexFlag == 0){ // 不以16进制显示
        do {
            -- tmp_last;
            *tmp_last = (u_char) (ui64 % 10 + '0');
        } while (ui64 /= 10); // 从后往前添加数据
    }
    else if(hexFlag == 1){ // 以小写字母的形式显示16进制整数，比如30，显示为1e
        do {            
            -- tmp_last;
            *tmp_last = hex[(uint32_t) (ui64 & 0xf)]; 
        } while (ui64 >>= 4);
    } 
    else{ // 以大写字母的形式显示16进制整数，比如30，显示为1E
        do { 
            -- tmp_last;
            *tmp_last = HEX[(uint32_t) (ui64 & 0xf)]; 
        } while (ui64 >>= 4);
    }

    u_int32_t len = (temp + NGX_INT64_LEN) - tmp_last; // temp填有数据的长度，比如123，len = 3
    while (len < width){ //如果希望显示的宽度是%12d，而实际的数是7654321，只有7个宽度，那么这里要填充5个0
        -- tmp_last;
        *tmp_last = padding;
        ++ len;
    }

    if((buf + len) >= last){
        len = last - buf; //剩余的buf有多少字节就拷贝多少
    }

    return ngx_memcpy(buf, tmp_last, len);
}