//stamp:0755f90cf3e34c44
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
			}
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
		{L"btn_browse_comp",65541},
		{L"btn_browse_keymap",65545},
		{L"btn_browse_logo",65547},
		{L"btn_browse_save",65543},
		{L"btn_build",65555},
		{L"btn_close",65536},
		{L"btn_export",65556},
		{L"btn_max",65537},
		{L"btn_min",65539},
		{L"btn_restore",65538},
		{L"chk_auto_select",65550},
		{L"chk_phrase_full_code",65551},
		{L"chk_symbol_first",65548},
		{L"chk_yinxingma",65549},
		{L"edit_cit_save",65542},
		{L"edit_comp",65540},
		{L"edit_comp_logo",65546},
		{L"edit_keymap",65544},
		{L"edit_url",65552},
		{L"img_keymap",65554},
		{L"img_logo",65553}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			btn_browse_comp = namedXmlID[1].strName;
			btn_browse_keymap = namedXmlID[2].strName;
			btn_browse_logo = namedXmlID[3].strName;
			btn_browse_save = namedXmlID[4].strName;
			btn_build = namedXmlID[5].strName;
			btn_close = namedXmlID[6].strName;
			btn_export = namedXmlID[7].strName;
			btn_max = namedXmlID[8].strName;
			btn_min = namedXmlID[9].strName;
			btn_restore = namedXmlID[10].strName;
			chk_auto_select = namedXmlID[11].strName;
			chk_phrase_full_code = namedXmlID[12].strName;
			chk_symbol_first = namedXmlID[13].strName;
			chk_yinxingma = namedXmlID[14].strName;
			edit_cit_save = namedXmlID[15].strName;
			edit_comp = namedXmlID[16].strName;
			edit_comp_logo = namedXmlID[17].strName;
			edit_keymap = namedXmlID[18].strName;
			edit_url = namedXmlID[19].strName;
			img_keymap = namedXmlID[20].strName;
			img_logo = namedXmlID[21].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * btn_browse_comp;
		 const wchar_t * btn_browse_keymap;
		 const wchar_t * btn_browse_logo;
		 const wchar_t * btn_browse_save;
		 const wchar_t * btn_build;
		 const wchar_t * btn_close;
		 const wchar_t * btn_export;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restore;
		 const wchar_t * chk_auto_select;
		 const wchar_t * chk_phrase_full_code;
		 const wchar_t * chk_symbol_first;
		 const wchar_t * chk_yinxingma;
		 const wchar_t * edit_cit_save;
		 const wchar_t * edit_comp;
		 const wchar_t * edit_comp_logo;
		 const wchar_t * edit_keymap;
		 const wchar_t * edit_url;
		 const wchar_t * img_keymap;
		 const wchar_t * img_logo;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int btn_browse_comp	=	65541;
		const static int btn_browse_keymap	=	65545;
		const static int btn_browse_logo	=	65547;
		const static int btn_browse_save	=	65543;
		const static int btn_build	=	65555;
		const static int btn_close	=	65536;
		const static int btn_export	=	65556;
		const static int btn_max	=	65537;
		const static int btn_min	=	65539;
		const static int btn_restore	=	65538;
		const static int chk_auto_select	=	65550;
		const static int chk_phrase_full_code	=	65551;
		const static int chk_symbol_first	=	65548;
		const static int chk_yinxingma	=	65549;
		const static int edit_cit_save	=	65542;
		const static int edit_comp	=	65540;
		const static int edit_comp_logo	=	65546;
		const static int edit_keymap	=	65544;
		const static int edit_url	=	65552;
		const static int img_keymap	=	65554;
		const static int img_logo	=	65553;
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
