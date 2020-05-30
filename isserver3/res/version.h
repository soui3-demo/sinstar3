#pragma once

//
// 我们把版本信息修改集中到这一处地方。
// 程序中使用的内核对象名称，版本信息都是以宏定义，都根据4个版本信息字段展开生成，
// 要改变版本号，只需要对这4个字段进行修改。
//

#define COPYRIGHT_STR "Copy right (C) 2003-2020"
#define _sinstar3_ime_version_a		3
#define _sinstar3_ime_version_b		1
#define _sinstar3_ime_version_c		3
#define _sinstar3_ime_version_d		9


#define _sinstar3_ime_int_version(a,b,c,d)		a,b,c,d
#define sinstar3_ime_int_version(a,b,c,d)		_sinstar3_ime_int_version(a,b,c,d)

#define _sinstar3_ime_str_version(a,b,c,d)		#a "." #b "." #c "." #d
#define sinstar3_ime_str_version(a,b,c,d)		_sinstar3_ime_str_version(a,b,c,d)

#define _sinstar3_ime_str_version2(a,b,c,d)	_T(#a) _T(".") _T(#b) _T(".") _T(#c) _T(".") _T(#d)
#define sinstar3_ime_str_version2(a,b,c,d)		_sinstar3_ime_str_version2(a,b,c,d)

#define VERSION_INT		sinstar3_ime_int_version( _sinstar3_ime_version_a, _sinstar3_ime_version_b, _sinstar3_ime_version_c, _sinstar3_ime_version_d)
#define VERSION_STR		sinstar3_ime_str_version( _sinstar3_ime_version_a, _sinstar3_ime_version_b, _sinstar3_ime_version_c, _sinstar3_ime_version_d)


