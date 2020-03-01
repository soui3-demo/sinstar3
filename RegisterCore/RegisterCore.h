// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 REGISTERCORE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// REGISTERCORE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef REGISTERCORE_EXPORTS
#define REGISTERCORE_API __declspec(dllexport) 
#else
#define REGISTERCORE_API __declspec(dllimport) 
#endif

EXTERN_C{
	void REGISTERCORE_API WINAPI  Sinstar_InitW(LPCWSTR pszPath);
	void REGISTERCORE_API WINAPI  Sinstar_InitA(LPCSTR pszPath);
	LPCWSTR REGISTERCORE_API WINAPI  Sinstar_GetErrMsgW();
	LPCSTR REGISTERCORE_API WINAPI  Sinstar_GetErrMsgA();
	BOOL REGISTERCORE_API WINAPI  Sinstar_ShowCaller();
	BOOL REGISTERCORE_API WINAPI  Sinstar_IsRunning();
	BOOL REGISTERCORE_API WINAPI  Sinstar_Update();
	BOOL REGISTERCORE_API WINAPI  Sinstar_Uninstall();
	BOOL REGISTERCORE_API WINAPI  Sinstar_ForceUninstall();
	BOOL REGISTERCORE_API WINAPI  Sinstar_Install();
	BOOL REGISTERCORE_API WINAPI  Sinstar_GetCurrentVer(int wVers[4]);
	BOOL REGISTERCORE_API WINAPI  Sinstar_GetCurrentVer2(int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersionW(LPCWSTR pszFileName, int wVers[4]);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersionA(LPCSTR pszFileName, int wVers[4]);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersion2W(LPCWSTR pszFileName, int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API WINAPI  Sinstar_PEVersion2A(LPCSTR pszFileName, int *v1,int *v2,int *v3,int *v4);
	BOOL REGISTERCORE_API WINAPI  Sinstar_CheckFiles();
};

#ifdef _UNICODE
#define Sinstar_PEVersion Sinstar_PEVersionW
#define Sinstar_Init	  Sinstar_InitW
#define Sinstar_GetErrMsg Sinstar_GetErrMsgW
#else 
#define Sinstar_PEVersion Sinstar_PEVersionA
#define Sinstar_Init	  Sinstar_InitA
#define Sinstar_GetErrMsg Sinstar_GetErrMsgA
#endif
