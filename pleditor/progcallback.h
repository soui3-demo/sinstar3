#ifndef _PROGCALLBACK_
#define _PROGCALLBACK_

typedef enum tagPROGTYPE{PT_MAX=0,PT_PROG,PT_END} PROGTYPE;
typedef void (*FunProgCallback)(PROGTYPE uType,DWORD dwData,LPARAM lParam);

#endif//_PROGCALLBACK_