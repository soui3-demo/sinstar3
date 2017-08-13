#pragma once


//
// for 1.0.5.0 以前的版本
//
#define BAIDU_SYS_CONFIG_FILE_NAME_1_0_5_0	_T("baidujp.cfg")
#define	BAIDU_IME_FILE_NAME_1_0_5_0			_T("baidujp.ime")

#define BAIDU_JP_IME_SERV_NAME_1_0_5_0	_T("BaiduJP_IME_Service_1.0.5.0")
#define BAIDU_JP_IME_SERV_NAME_1_0_5_1	_T("BaiduJP_IME_Service_1.0.5.1")
#define BAIDU_JP_IME_SERV_NAME_2_0_0_1	_T("BaiduJP_IME_Service_2.0.0.1")
#define BAIDU_JP_IME_SERV_NAME_2_0_0_2	_T("BaiduJP_IME_Service_2.0.0.2")
#define BAIDU_JP_IME_SERV_NAME_2_0_0_3	_T("BaiduJP_IME_Service_2.0.0.3")
#define BAIDU_JP_IME_SERV_NAME			_T("BaiduJP_IME_Service")

#define BAIDU_JP_OLD_VERSION_INI		_T("baidujp_old_version.ini")

//
// 当前版本
//
#define NAME_FLAG_IMERUNNING		_T("baidujp_event_running_")BAIDUJP_IME_VERSION	//输入法使用中的标志
#define NAME_IME_COMM_MUTEX			_T("BAIDUJPIME_MUTEX_{1E465CD4-59DB-4024-BE99-86FF0ABF0338}_")BAIDUJP_IME_VERSION
#define NAME_IME_COMM_MEMMAPFILE	_T("BAIDUJPIME_MEMMAPFILE_{1E465CD4-59DB-4024-BE99-86FF0ABF0338}_")BAIDUJP_IME_VERSION
#define NAME_IME_COMM_MSG			_T("BAIDUJPIME_MSG_{1E465CD4-59DB-4024-BE99-86FF0ABF0338}_")BAIDUJP_IME_VERSION
#define NAME_IME_INI_MUTEX			_T("BaiduJPIMEConverter-{F88B196A-577F-40a7-95AF-F529B046CA30}_")BAIDUJP_IME_VERSION

// baidujpengine, baidujp3, baidujpcom都使用了这个共享内存映射文件
// baidujpengine, baidujp3中捕获到的异常dump到这个共享内存映射文件中，baidujpcom从中读取dump上传
#define MMN_BUGREPORT				_T("BaiduJP_IME_BUGREPORT_{B1C7511A-AF53-4a92-B17D-2A17EE65C58F}_")BAIDUJP_IME_VERSION

#define JP_IME_CONVERTER_TITLE		_T("BaiduJP IME Converter_2.0.0.2")
#define JP_IME_CONVERTER_CLASS      _T("BaiduJPIMEConverter-{F88B196A-577F-40a7-95AF-F529B046CA30}_")BAIDUJP_IME_VERSION

#define JP_IME_DESC					_T("Baidu IME")
#define JP_IME_DESC_W				L"Baidu IME"

#define	BAIDU_IME_FILE_NAME			_T("baidujp20.ime")

#define BAIDU_SYS_CONFIG_FILE_NAME	_T("baidujp20.cfg")

// UICLASSNAME的长度最多为15个字符
#define UICLASSNAME					_T("BDJP_2.0")

#define TEXTSERVICE_LANGID			MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN)

#define IME_INSTALLER_MUTEX			_T("baidu_ime_install_mutex_")BAIDUJP_IME_VERSION

#define BAIDU_JP_IME_SERV_DISPLAY_NAME	_T("Baidu Japanese IME Service_")BAIDUJP_IME_VERSION
#define	BAIDU_JP_IME_SERVICE_MUTEX		_T("Global\\BaiduJP_IME_Service_Mutex_")BAIDUJP_IME_VERSION
#define BAIDU_JP_IME_PIPE				_T("\\\\.\\pipe\\BaiduJP_IME_Service_Pipe_")BAIDUJP_IME_VERSION
#define BAIDU_JP_IME_DATA_PIPE			_T("\\\\.\\pipe\\BaiduJP_IME_Data_Pipe_")BAIDUJP_IME_VERSION
#define BAIDU_JP_IME_PROMPT_MUTEX		_T("BaiduJP_IME_Prompt_Mutex_")BAIDUJP_IME_VERSION
#define BAIDU_JP_IME_SERV_AGENT_MUTEX	_T("BaiduJP_IME_Serv_Agent_Mutex_")BAIDUJP_IME_VERSION

#define URL_UPLOAD		_T("http://sync.ime.baidu.jp/cgi-bin/getlog.cgi")
#define URL_UPLOADTEXT	_T("http://sync.ime.baidu.jp/cgi-bin/getmsg.cgi")