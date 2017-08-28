// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <BaseTyps.h>
#include <Unknwn.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件
#include "resource.h"

#include "../include/sinstar-i.h"
#include "../include/TextService-i.h"
#include "../include/unknownimpl.h"


#define LOG_FILTER "sinstar3_core"

#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <com-cfg.h>
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"


#include "controls/SToggle2.h"
#include "controls/SCandView.h"
#include "controls/SCaption2.h"

#include "dataCenter/datacenter.h"
using namespace SOUI;

#include "sinstar3_core.h"
