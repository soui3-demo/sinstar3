#pragma once

//
// 我们把版本信息修改集中到这一处地方。
// 程序中使用的内核对象名称，版本信息都是以宏定义，都根据4个版本信息字段展开生成，
// 要改变版本号，只需要对这4个字段进行修改。
//

#define _baidu_ime_version_a		3
#define _baidu_ime_version_b		0
#define _baidu_ime_version_c		0
#define _baidu_ime_version_d		1

#define _baidu_ime_name				启程输入之星


#define _baidu_ime_string(a)		#a
#define baidu_ime_string(a)			_baidu_ime_string(a)
#define _baidu_ime_tstring(a)		_T(#a)
#define baidu_ime_tstring(a)		_baidu_ime_tstring(a)

#define _baidu_ime_int_version(a,b,c,d)		a,b,c,d
#define baidu_ime_int_version(a,b,c,d)		_baidu_ime_int_version(a,b,c,d)

#define _baidu_ime_str_version(a,b,c,d)		#a "." #b "." #c "." #d
#define baidu_ime_str_version(a,b,c,d)		_baidu_ime_str_version(a,b,c,d)

#define _baidu_ime_str_version2(a,b,c,d)	_T(#a) _T(".") _T(#b) _T(".") _T(#c) _T(".") _T(#d)
#define baidu_ime_str_version2(a,b,c,d)		_baidu_ime_str_version2(a,b,c,d)

#define BAIDUJP_IME_VERSION		baidu_ime_str_version2( _baidu_ime_version_a, _baidu_ime_version_b, _baidu_ime_version_c, _baidu_ime_version_d)
#define VERSION_INT		baidu_ime_int_version( _baidu_ime_version_a, _baidu_ime_version_b, _baidu_ime_version_c, _baidu_ime_version_d)
#define VERSION_STR		baidu_ime_str_version( _baidu_ime_version_a, _baidu_ime_version_b, _baidu_ime_version_c, _baidu_ime_version_d)
#define PRODUCT_NAME	baidu_ime_tstring( _baidu_ime_name)
