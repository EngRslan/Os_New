#include <ctype.h>

int isalnum(int c){
    return isalpha(c) || isdigit(c);
}
int isalpha(int c){
    return islower(c) || isupper(c);
}
int iscntrl(int c){
    return (c >= 0x0 && c <= 0x1F) || c == 0x7f;
}
int isdigit(int c){
    return c >= '0' && c <= '9' ;
}
int isgraph(int c){
    return isalnum(c) || ispunct(c);
}
int islower(int c){
    return c >= 'a' && c <= 'z';
}
int isprint(int c){
    return isgraph(c) || isspace(c);
}
int ispunct(int c){
    //           '!'             '/'         ':'          '#'           '['          '''          '{'          '~'
    return (c >= 0x21 && c <= 0x2f) || (c >= 0x3a && c <= 0x40) ||(c >= 0x5b && c <= 0x60)||(c >= 0x7b && c <= 0x7e);
}
int isspace(int c){
    return (c >= 0x09 && c <= 0x0d)|| c == 0x20;
}
int isupper(int c){
    return c >= 'A' && c <= 'Z';
}
int isxdigit(int c){
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}
int tolower(int c){
    if(!isupper(c)) return c;
    return c + 0x20;
}
int toupper(int c){
    if(!islower(c)) return c;
    return c - 0x20;
}