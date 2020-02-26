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
	void REGISTERCORE_API Sinstar_Init(LPCTSTR pszPath);
	LPCTSTR REGISTERCORE_API Sinstar_GetErrMsg();
	BOOL REGISTERCORE_API Sinstar_ShowCaller();
	BOOL REGISTERCORE_API Sinstar_IsRunning();
	BOOL REGISTERCORE_API Sinstar_Update();
	BOOL REGISTERCORE_API Sinstar_Uninstall();
	BOOL REGISTERCORE_API Sinstar_ForceUninstall();
	BOOL REGISTERCORE_API Sinstar_Install();
	BOOL REGISTERCORE_API Sinstar_GetCurrentVer(WORD wVers[4]);
	BOOL REGISTERCORE_API Sinstar_PEVersion(LPCTSTR pszFileName, WORD wVers[4]);
	BOOL REGISTERCORE_API Sinstar_CheckFiles();
};
