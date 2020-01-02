#ifndef _PROGCALLBACK_
#define _PROGCALLBACK_

typedef enum tagPROGTYPE{PT_MAX=0,PT_PROG,PT_END} PROGTYPE;
struct IProgListener{
	virtual void OnProg(PROGTYPE uType,DWORD dwData) = 0;
};

#endif//_PROGCALLBACK_