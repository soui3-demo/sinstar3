#pragma  once


#define UM_UPDATESKIN		(WM_USER+100)	//皮肤更改消息
#define UM_BTNCHANGED		(WM_USER+101)
#define UM_DRAGEND			(WM_USER+102)
#define UM_UPDATEPAGEBTN	(WM_USER+200)
#define UM_VIEWMODE			(WM_USER+900)

//支持输入法动态更新的命名消息:wParam: 0-uninstall,1-reload
#define MSG_UPDATE_BAIDUJP3			_T("MSG_UPDATE_BAIDUJP3_")

//设置修改前的消息,要求所有窗口结束当前输入，wParam,lParam,没有定义，设置为0
#define MSG_ENDCOMPOSITION_BAIDUJP3 _T("MSG_ENDCOMPOSITION_BAIDUJP3_")

//设置修改后的更新消息,wParam,lParam,没有定义，设置为0
#define MSG_SETTINGSCHANGED_BAIDUJP3 _T("MSG_SETTINGSCHANGED_BAIDUJP3_")

//注册的皮肤切换广播消息
#define	MSG_SKINCHANGED_BAIDUJP3	_T("_Baidu type skin change message_")

// 注册的字典切换广播消息 李贤辉 2010-12-17
#define MSG_DICTCHANGED_BAIDUJP3	_T("_Baidu type dict change message_")

#define MSGLISTNERWNDCLASS			_T("BAIDUJP_MSGLISTNER_WND_")
