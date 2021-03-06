//stamp:0754a8c249889401
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
			}
			const TCHAR * XML_MAINWND;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMG{
			public:
			_IMG(){
				png_item_bg = _T("IMG:png_item_bg");
			}
			const TCHAR * png_item_bg;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"btn_activate",65548},
		{L"btn_close",65536},
		{L"btn_kill",65549},
		{L"btn_max",65537},
		{L"btn_min",65539},
		{L"btn_refresh",65547},
		{L"btn_restore",65538},
		{L"col_name",65543},
		{L"col_path",65545},
		{L"col_pid",65541},
		{L"mclv_tasks",65540},
		{L"txt_name",65544},
		{L"txt_path",65546},
		{L"txt_pid",65542}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			btn_activate = namedXmlID[1].strName;
			btn_close = namedXmlID[2].strName;
			btn_kill = namedXmlID[3].strName;
			btn_max = namedXmlID[4].strName;
			btn_min = namedXmlID[5].strName;
			btn_refresh = namedXmlID[6].strName;
			btn_restore = namedXmlID[7].strName;
			col_name = namedXmlID[8].strName;
			col_path = namedXmlID[9].strName;
			col_pid = namedXmlID[10].strName;
			mclv_tasks = namedXmlID[11].strName;
			txt_name = namedXmlID[12].strName;
			txt_path = namedXmlID[13].strName;
			txt_pid = namedXmlID[14].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * btn_activate;
		 const wchar_t * btn_close;
		 const wchar_t * btn_kill;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_refresh;
		 const wchar_t * btn_restore;
		 const wchar_t * col_name;
		 const wchar_t * col_path;
		 const wchar_t * col_pid;
		 const wchar_t * mclv_tasks;
		 const wchar_t * txt_name;
		 const wchar_t * txt_path;
		 const wchar_t * txt_pid;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int btn_activate	=	65548;
		const static int btn_close	=	65536;
		const static int btn_kill	=	65549;
		const static int btn_max	=	65537;
		const static int btn_min	=	65539;
		const static int btn_refresh	=	65547;
		const static int btn_restore	=	65538;
		const static int col_name	=	65543;
		const static int col_path	=	65545;
		const static int col_pid	=	65541;
		const static int mclv_tasks	=	65540;
		const static int txt_name	=	65544;
		const static int txt_path	=	65546;
		const static int txt_pid	=	65542;
		}id;

		class _string{
		public:
		const static int title	=	0;
		const static int ver	=	1;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int gray	=	1;
		const static int green	=	2;
		const static int red	=	3;
		const static int white	=	4;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
