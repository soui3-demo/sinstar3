#pragma once

//
// 我们把版本信息修改集中到这一处地方。
// 程序中使用的内核对象名称，版本信息都是以宏定义，都根据4个版本信息字段展开生成，
// 要改变版本号，只需要对这4个字段进行修改。
//

#define _sinstar3_ime_version_a		3
#define _sinstar3_ime_version_b		0
#define _sinstar3_ime_version_c		0
#define _sinstar3_ime_version_d		1

#define _sinstar3_ime_name			启程输入法
#define _sinstar3_ime				启程输入法IME模块
#define _sinstar3_tsf				       启程输入法TSF模块
#define _sinstar3_mutex                       sinstar3_mutex_40AC2365-F592-4BE3-8B41-04A6B5337BFB


#define _sinstar3_ime_string(a)		#a
#define sinstar3_ime_string(a)			_sinstar3_ime_string(a)
#define _sinstar3_ime_tstring(a)		_T(#a)
#define sinstar3_ime_tstring(a)		_sinstar3_ime_tstring(a)
#define _sinstar3_ime_wstring(a)		L#a
#define sinstar3_ime_wstring(a)		_sinstar3_ime_wstring(a)

#define _sinstar3_ime_int_version(a,b,c,d)		a,b,c,d
#define sinstar3_ime_int_version(a,b,c,d)		_sinstar3_ime_int_version(a,b,c,d)

#define _sinstar3_ime_str_version(a,b,c,d)		#a "." #b "." #c "." #d
#define sinstar3_ime_str_version(a,b,c,d)		_sinstar3_ime_str_version(a,b,c,d)

#define _sinstar3_ime_str_version2(a,b,c,d)	_T(#a) _T(".") _T(#b) _T(".") _T(#c) _T(".") _T(#d)
#define sinstar3_ime_str_version2(a,b,c,d)		_sinstar3_ime_str_version2(a,b,c,d)

#define VERSION_INT		sinstar3_ime_int_version( _sinstar3_ime_version_a, _sinstar3_ime_version_b, _sinstar3_ime_version_c, _sinstar3_ime_version_d)
#define VERSION_STR		sinstar3_ime_str_version( _sinstar3_ime_version_a, _sinstar3_ime_version_b, _sinstar3_ime_version_c, _sinstar3_ime_version_d)
#define VERSION_TSTR	sinstar3_ime_str_version2( _sinstar3_ime_version_a, _sinstar3_ime_version_b, _sinstar3_ime_version_c, _sinstar3_ime_version_d)
#define PRODUCT_NAME	sinstar3_ime_tstring( _sinstar3_ime_name)
#define PRODUCT_WNAME	sinstar3_ime_wstring( _sinstar3_ime_name)

#define PRODUCT_IME	sinstar3_ime_tstring( _sinstar3_ime)
#define PRODUCT_TSF	sinstar3_ime_wstring( _sinstar3_tsf)
#define SINSTAR3_MUTEX sinstar3_ime_tstring(_sinstar3_mutex)

#define _sinstar3_ime_name_ver(n,a,b)	_T(#n) _T(#a) _T(".") _T(#b)

#define PRODUCT_NAMEANDVER _sinstar3_ime_name_ver(_sinstar3_ime_name,_sinstar3_ime_version_a,_sinstar3_ime_version_b)
