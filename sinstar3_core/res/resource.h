//stamp:0ccaa02b16dc3dd4
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
				wnd_composition = _T("LAYOUT:wnd_composition");
				wnd_status_bar = _T("LAYOUT:wnd_status_bar");
				dlg_config = _T("LAYOUT:dlg_config");
			}
			const TCHAR * wnd_composition;
			const TCHAR * wnd_status_bar;
			const TCHAR * dlg_config;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
			}
			const TCHAR * string;
			const TCHAR * color;
		}values;
		class _IMG{
			public:
			_IMG(){
				logo_32 = _T("IMG:logo_32");
				png_charmode = _T("IMG:png_charmode");
				png_charmode_full = _T("IMG:png_charmode_full");
				png_charmode_half = _T("IMG:png_charmode_half");
				png_status_open = _T("IMG:png_status_open");
				png_status_close = _T("IMG:png_status_close");
				png_composition = _T("IMG:png_composition");
				btn_extend = _T("IMG:btn_extend");
				btn_shrink = _T("IMG:btn_shrink");
				btn_makeword = _T("IMG:btn_makeword");
				btn_prevpage = _T("IMG:btn_prevpage");
				btn_nextpage = _T("IMG:btn_nextpage");
				btn_query = _T("IMG:btn_query");
				btn_record_enable = _T("IMG:btn_record_enable");
				btn_record_disable = _T("IMG:btn_record_disable");
				btn_sound_enable = _T("IMG:btn_sound_enable");
				btn_sound_disable = _T("IMG:btn_sound_disable");
			}
			const TCHAR * logo_32;
			const TCHAR * png_charmode;
			const TCHAR * png_charmode_full;
			const TCHAR * png_charmode_half;
			const TCHAR * png_status_open;
			const TCHAR * png_status_close;
			const TCHAR * png_composition;
			const TCHAR * btn_extend;
			const TCHAR * btn_shrink;
			const TCHAR * btn_makeword;
			const TCHAR * btn_prevpage;
			const TCHAR * btn_nextpage;
			const TCHAR * btn_query;
			const TCHAR * btn_record_enable;
			const TCHAR * btn_record_disable;
			const TCHAR * btn_sound_enable;
			const TCHAR * btn_sound_disable;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _smenu{
			public:
			_smenu(){
				context = _T("smenu:context");
			}
			const TCHAR * context;
		}smenu;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"btn_charmode",65559},
		{L"btn_close",1},
		{L"btn_makeword",65560},
		{L"btn_nextpage",65548},
		{L"btn_prevpage",65546},
		{L"btn_record",65561},
		{L"btn_sound",65562},
		{L"btn_status_extend",65556},
		{L"btn_status_shrink",65557},
		{L"cand_container",65547},
		{L"cand_english",65549},
		{L"cand_normal",65545},
		{L"cand_tip",65551},
		{L"cfg_about",2004},
		{L"cfg_associate",2002},
		{L"cfg_candidate",2003},
		{L"cfg_habit",2000},
		{L"cfg_hotkey",2001},
		{L"comp_normal",65536},
		{L"comp_sent",65541},
		{L"comp_spell",65538},
		{L"comp_umode",65540},
		{L"img_logo",65554},
		{L"sent_input",65542},
		{L"sent_left",65543},
		{L"sent_right",65544},
		{L"status_base",65553},
		{L"status_bg",65552},
		{L"status_extend",65558},
		{L"sv_setting",65563},
		{L"title_about",1004},
		{L"title_associate",1002},
		{L"title_candidate",1003},
		{L"title_habit",1000},
		{L"title_hotkey",1001},
		{L"txt_comp",65555},
		{L"txt_comps",65537},
		{L"txt_en_header",65550},
		{L"txt_temp_spell_flag",65539}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			btn_charmode = namedXmlID[1].strName;
			btn_close = namedXmlID[2].strName;
			btn_makeword = namedXmlID[3].strName;
			btn_nextpage = namedXmlID[4].strName;
			btn_prevpage = namedXmlID[5].strName;
			btn_record = namedXmlID[6].strName;
			btn_sound = namedXmlID[7].strName;
			btn_status_extend = namedXmlID[8].strName;
			btn_status_shrink = namedXmlID[9].strName;
			cand_container = namedXmlID[10].strName;
			cand_english = namedXmlID[11].strName;
			cand_normal = namedXmlID[12].strName;
			cand_tip = namedXmlID[13].strName;
			cfg_about = namedXmlID[14].strName;
			cfg_associate = namedXmlID[15].strName;
			cfg_candidate = namedXmlID[16].strName;
			cfg_habit = namedXmlID[17].strName;
			cfg_hotkey = namedXmlID[18].strName;
			comp_normal = namedXmlID[19].strName;
			comp_sent = namedXmlID[20].strName;
			comp_spell = namedXmlID[21].strName;
			comp_umode = namedXmlID[22].strName;
			img_logo = namedXmlID[23].strName;
			sent_input = namedXmlID[24].strName;
			sent_left = namedXmlID[25].strName;
			sent_right = namedXmlID[26].strName;
			status_base = namedXmlID[27].strName;
			status_bg = namedXmlID[28].strName;
			status_extend = namedXmlID[29].strName;
			sv_setting = namedXmlID[30].strName;
			title_about = namedXmlID[31].strName;
			title_associate = namedXmlID[32].strName;
			title_candidate = namedXmlID[33].strName;
			title_habit = namedXmlID[34].strName;
			title_hotkey = namedXmlID[35].strName;
			txt_comp = namedXmlID[36].strName;
			txt_comps = namedXmlID[37].strName;
			txt_en_header = namedXmlID[38].strName;
			txt_temp_spell_flag = namedXmlID[39].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * btn_charmode;
		 const wchar_t * btn_close;
		 const wchar_t * btn_makeword;
		 const wchar_t * btn_nextpage;
		 const wchar_t * btn_prevpage;
		 const wchar_t * btn_record;
		 const wchar_t * btn_sound;
		 const wchar_t * btn_status_extend;
		 const wchar_t * btn_status_shrink;
		 const wchar_t * cand_container;
		 const wchar_t * cand_english;
		 const wchar_t * cand_normal;
		 const wchar_t * cand_tip;
		 const wchar_t * cfg_about;
		 const wchar_t * cfg_associate;
		 const wchar_t * cfg_candidate;
		 const wchar_t * cfg_habit;
		 const wchar_t * cfg_hotkey;
		 const wchar_t * comp_normal;
		 const wchar_t * comp_sent;
		 const wchar_t * comp_spell;
		 const wchar_t * comp_umode;
		 const wchar_t * img_logo;
		 const wchar_t * sent_input;
		 const wchar_t * sent_left;
		 const wchar_t * sent_right;
		 const wchar_t * status_base;
		 const wchar_t * status_bg;
		 const wchar_t * status_extend;
		 const wchar_t * sv_setting;
		 const wchar_t * title_about;
		 const wchar_t * title_associate;
		 const wchar_t * title_candidate;
		 const wchar_t * title_habit;
		 const wchar_t * title_hotkey;
		 const wchar_t * txt_comp;
		 const wchar_t * txt_comps;
		 const wchar_t * txt_en_header;
		 const wchar_t * txt_temp_spell_flag;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int btn_charmode	=	65559;
		const static int btn_close	=	1;
		const static int btn_makeword	=	65560;
		const static int btn_nextpage	=	65548;
		const static int btn_prevpage	=	65546;
		const static int btn_record	=	65561;
		const static int btn_sound	=	65562;
		const static int btn_status_extend	=	65556;
		const static int btn_status_shrink	=	65557;
		const static int cand_container	=	65547;
		const static int cand_english	=	65549;
		const static int cand_normal	=	65545;
		const static int cand_tip	=	65551;
		const static int cfg_about	=	2004;
		const static int cfg_associate	=	2002;
		const static int cfg_candidate	=	2003;
		const static int cfg_habit	=	2000;
		const static int cfg_hotkey	=	2001;
		const static int comp_normal	=	65536;
		const static int comp_sent	=	65541;
		const static int comp_spell	=	65538;
		const static int comp_umode	=	65540;
		const static int img_logo	=	65554;
		const static int sent_input	=	65542;
		const static int sent_left	=	65543;
		const static int sent_right	=	65544;
		const static int status_base	=	65553;
		const static int status_bg	=	65552;
		const static int status_extend	=	65558;
		const static int sv_setting	=	65563;
		const static int title_about	=	1004;
		const static int title_associate	=	1002;
		const static int title_candidate	=	1003;
		const static int title_habit	=	1000;
		const static int title_hotkey	=	1001;
		const static int txt_comp	=	65555;
		const static int txt_comps	=	65537;
		const static int txt_en_header	=	65550;
		const static int txt_temp_spell_flag	=	65539;
		}id;

		class _string{
		public:
		const static int btn_close	=	0;
		const static int cfg_about	=	1;
		const static int cfg_associate	=	2;
		const static int cfg_candidate	=	3;
		const static int cfg_habit	=	4;
		const static int cfg_hotkey	=	5;
		const static int txt_config	=	6;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int cand_comp	=	1;
		const static int cand_index	=	2;
		const static int cand_text	=	3;
		const static int cand_wild	=	4;
		const static int gray	=	5;
		const static int green	=	6;
		const static int red	=	7;
		const static int sent_input	=	8;
		const static int sent_left	=	9;
		const static int sent_right	=	10;
		const static int white	=	11;
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
