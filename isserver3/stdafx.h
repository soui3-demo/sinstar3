// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif//_CRT_SECURE_NO_WARNINGS

#define  _CRT_SECURE_NO_WARNINGS
// Windows 头文件:
#include <windows.h>
#include <BaseTyps.h>
#include <Unknwn.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件
#include "resource.h"

#include "../include/sinstar-i.h"
#include "../include/TextService-i.h"
#include "../include/unknown.h"

#define LOG_FILTER "sinstar3_core"

#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <event/NotifyCenter.h>
#include <commgr2.h>
#include <helper/SMenu.h>
#include <resprovider-zip/zipresprovider-param.h>

#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;

#include "../include/reg.h"
#include "../iscomm/isProtocol.h"
#include "../iscomm/iscomm.h"

#include "global_def.h"

#include "ime/inputContext.h"
#include "ime/CmdHandler.h"

#include "controls/SChatEdit.h"
#include "controls/SToggle2.h"
#include "controls/SCandView.h"
#include "controls/SSentView.h"
#include "controls/SEnglishCand.h"
#include "controls/SPhraseCand.h"
#include "controls/SCaption2.h"
#include "controls/SCompBackground.h"
#include "controls/SStatusBackground.h"
#include "controls/SMutexView.h"
#include "controls/SSpellView.h"
#include "controls/SFlagView.h"
#include "controls/SLineComp.h"
#include "controls/SApngPlayer.h"

#include "controls/SSkinAPNG.h"
#include "controls/SSkinPNGX.h"
#include "controls/SLineSkin.h"
#include "controls/SQrCtrl.h"
#include "controls/STabCtrlEx.h"

#include "dataCenter/datacenter.h"
#include "dataCenter/Settings.h"


extern SComMgr2 * g_ComMgr2;