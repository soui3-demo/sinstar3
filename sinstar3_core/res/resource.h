//stamp:1075bd5360dd4859
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
				dlg_config = _T("LAYOUT:dlg_config");
				wnd_composition = _T("LAYOUT:wnd_composition");
				wnd_status_bar = _T("LAYOUT:wnd_status_bar");
				wnd_tip = _T("LAYOUT:wnd_tip");
				wnd_spchar = _T("LAYOUT:wnd_spchar");
			}
			const TCHAR * dlg_config;
			const TCHAR * wnd_composition;
			const TCHAR * wnd_status_bar;
			const TCHAR * wnd_tip;
			const TCHAR * wnd_spchar;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
				dim = _T("values:dim");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
			const TCHAR * dim;
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
				SYS_SCROLLBAR = _T("IMG:SYS_SCROLLBAR");
				png_lines = _T("IMG:png_lines");
				btn_english_enable = _T("IMG:btn_english_enable");
				btn_english_disable = _T("IMG:btn_english_disable");
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
			const TCHAR * SYS_SCROLLBAR;
			const TCHAR * png_lines;
			const TCHAR * btn_english_enable;
			const TCHAR * btn_english_disable;
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
		{L"ass_mode_english_prompt",402},
		{L"ass_mode_none",400},
		{L"ass_mode_phrase",401},
		{L"ass_mode_phrase_prompt",403},
		{L"btn_charmode",65593},
		{L"btn_close",2},
		{L"btn_english",65597},
		{L"btn_filter_gbk",65598},
		{L"btn_help",65601},
		{L"btn_make_phrase",65594},
		{L"btn_menu",65600},
		{L"btn_nextpage",65580},
		{L"btn_prevpage",65578},
		{L"btn_query",65599},
		{L"btn_record",65595},
		{L"btn_sound",65596},
		{L"btn_status_extend",65589},
		{L"btn_status_shrink",65592},
		{L"cand_auto_input",65538},
		{L"cand_container",65579},
		{L"cand_english",65581},
		{L"cand_english_input",65584},
		{L"cand_normal",65577},
		{L"cand_phrase",65583},
		{L"cand_py_phrase_first",65539},
		{L"cand_tip",65585},
		{L"cell_00",100},
		{L"cell_01",101},
		{L"cell_02",102},
		{L"cell_03",103},
		{L"cell_04",104},
		{L"cell_05",105},
		{L"cell_06",106},
		{L"cell_07",107},
		{L"cell_08",108},
		{L"cell_09",109},
		{L"cell_10",110},
		{L"cell_11",111},
		{L"cell_12",112},
		{L"cell_13",113},
		{L"cell_14",114},
		{L"cell_15",115},
		{L"cell_16",116},
		{L"cell_17",117},
		{L"cell_18",118},
		{L"cell_19",119},
		{L"cell_20",120},
		{L"cell_21",121},
		{L"cell_22",122},
		{L"cell_23",123},
		{L"cell_24",124},
		{L"cell_25",125},
		{L"cell_26",126},
		{L"cell_27",127},
		{L"cell_28",128},
		{L"cell_29",129},
		{L"cell_30",130},
		{L"cell_31",131},
		{L"cell_32",132},
		{L"cell_33",133},
		{L"cell_34",134},
		{L"cell_35",135},
		{L"cell_36",136},
		{L"cell_37",137},
		{L"cell_38",138},
		{L"cell_39",139},
		{L"cell_40",140},
		{L"cell_41",141},
		{L"cell_42",142},
		{L"cell_43",143},
		{L"cell_44",144},
		{L"cell_45",145},
		{L"cell_46",146},
		{L"cell_47",147},
		{L"cell_48",148},
		{L"cell_49",149},
		{L"cell_parent",65607},
		{L"chk_auto_comp_promp",65553},
		{L"chk_auto_dot",65551},
		{L"chk_auto_select_cand",65552},
		{L"chk_disable_number_to_select_cand",65559},
		{L"chk_enable_23cand_hotkey",65556},
		{L"chk_full_skip_simple",65562},
		{L"chk_sent_input",65554},
		{L"chk_show_op_tip",65540},
		{L"comp_english",65574},
		{L"comp_lineime",65573},
		{L"comp_normal",65566},
		{L"comp_sent",65575},
		{L"comp_spell",65569},
		{L"comp_start",400},
		{L"comp_umode",65571},
		{L"config",100},
		{L"edit_webmode_header",65555},
		{L"enter_for_clear",110},
		{L"enter_for_input",111},
		{L"follow_caret",102},
		{L"forcast_always",601},
		{L"forcast_disable",600},
		{L"forcast_no_cand",602},
		{L"gbk_show_as_gb",702},
		{L"gbk_show_manual",701},
		{L"gbk_show_only",700},
		{L"help",112},
		{L"hide_statusbar",101},
		{L"hk_2_cand",65557},
		{L"hk_3_cand",65558},
		{L"hk_bihua_heng",900},
		{L"hk_bihua_na",903},
		{L"hk_bihua_pie",902},
		{L"hk_bihua_shu",901},
		{L"hk_bihua_wild",905},
		{L"hk_bihua_zhe",904},
		{L"hk_filter_gbk",65548},
		{L"hk_input_en",65547},
		{L"hk_make_phrase",65543},
		{L"hk_record",65550},
		{L"hk_show_comp",65545},
		{L"hk_show_statusbar",65546},
		{L"hk_show_table",65544},
		{L"hk_switch_py",65541},
		{L"hk_to_sentmode",65537},
		{L"hk_to_umode",65542},
		{L"hk_tts",65549},
		{L"hk_turn_next",65561},
		{L"hk_turn_prev",65560},
		{L"ime_switch_disabled",102},
		{L"ime_switch_left_shift",100},
		{L"ime_switch_right_shift",101},
		{L"img_logo",65587},
		{L"input_big5",103},
		{L"key_map",110},
		{L"key_speed",111},
		{L"key_to_sentmode",65536},
		{L"lst_catalog",65606},
		{L"memu_edit_symbol",702},
		{L"memu_edit_usercmd",703},
		{L"memu_edit_userdef",701},
		{L"memu_edit_userjm",704},
		{L"menu_blend",65609},
		{L"menu_comp",65608},
		{L"menu_data_mgr",65611},
		{L"menu_server",65610},
		{L"menu_skin",200},
		{L"open_spchar",104},
		{L"py_switch_disabled",122},
		{L"py_switch_left_ctrl",120},
		{L"py_switch_right_ctrl",121},
		{L"rate_adjust_auto",501},
		{L"rate_adjust_disable",500},
		{L"rate_adjust_fast",502},
		{L"skin_cloud",202},
		{L"skin_def",220},
		{L"skin_mgr",201},
		{L"sound_beep",802},
		{L"sound_disable",800},
		{L"sound_wave",801},
		{L"spell_all",504},
		{L"spell_one",501},
		{L"spell_three",503},
		{L"spell_two",502},
		{L"status_base",65591},
		{L"status_extend",65590},
		{L"status_shrink",65586},
		{L"stv_sent",65576},
		{L"svr_showicon",601},
		{L"txt_auto_complete",65572},
		{L"txt_build_time",65565},
		{L"txt_comp",65588},
		{L"txt_comps",65567},
		{L"txt_en_header",65582},
		{L"txt_svr_ver",65564},
		{L"txt_temp_spell_flag",65570},
		{L"txt_tip",65568},
		{L"txt_tip_content",65603},
		{L"txt_tip_title",65602},
		{L"txt_title",65605},
		{L"txt_ver",65563},
		{L"txt_zoomin",65604},
		{L"userdef",505}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			ass_mode_english_prompt = namedXmlID[1].strName;
			ass_mode_none = namedXmlID[2].strName;
			ass_mode_phrase = namedXmlID[3].strName;
			ass_mode_phrase_prompt = namedXmlID[4].strName;
			btn_charmode = namedXmlID[5].strName;
			btn_close = namedXmlID[6].strName;
			btn_english = namedXmlID[7].strName;
			btn_filter_gbk = namedXmlID[8].strName;
			btn_help = namedXmlID[9].strName;
			btn_make_phrase = namedXmlID[10].strName;
			btn_menu = namedXmlID[11].strName;
			btn_nextpage = namedXmlID[12].strName;
			btn_prevpage = namedXmlID[13].strName;
			btn_query = namedXmlID[14].strName;
			btn_record = namedXmlID[15].strName;
			btn_sound = namedXmlID[16].strName;
			btn_status_extend = namedXmlID[17].strName;
			btn_status_shrink = namedXmlID[18].strName;
			cand_auto_input = namedXmlID[19].strName;
			cand_container = namedXmlID[20].strName;
			cand_english = namedXmlID[21].strName;
			cand_english_input = namedXmlID[22].strName;
			cand_normal = namedXmlID[23].strName;
			cand_phrase = namedXmlID[24].strName;
			cand_py_phrase_first = namedXmlID[25].strName;
			cand_tip = namedXmlID[26].strName;
			cell_00 = namedXmlID[27].strName;
			cell_01 = namedXmlID[28].strName;
			cell_02 = namedXmlID[29].strName;
			cell_03 = namedXmlID[30].strName;
			cell_04 = namedXmlID[31].strName;
			cell_05 = namedXmlID[32].strName;
			cell_06 = namedXmlID[33].strName;
			cell_07 = namedXmlID[34].strName;
			cell_08 = namedXmlID[35].strName;
			cell_09 = namedXmlID[36].strName;
			cell_10 = namedXmlID[37].strName;
			cell_11 = namedXmlID[38].strName;
			cell_12 = namedXmlID[39].strName;
			cell_13 = namedXmlID[40].strName;
			cell_14 = namedXmlID[41].strName;
			cell_15 = namedXmlID[42].strName;
			cell_16 = namedXmlID[43].strName;
			cell_17 = namedXmlID[44].strName;
			cell_18 = namedXmlID[45].strName;
			cell_19 = namedXmlID[46].strName;
			cell_20 = namedXmlID[47].strName;
			cell_21 = namedXmlID[48].strName;
			cell_22 = namedXmlID[49].strName;
			cell_23 = namedXmlID[50].strName;
			cell_24 = namedXmlID[51].strName;
			cell_25 = namedXmlID[52].strName;
			cell_26 = namedXmlID[53].strName;
			cell_27 = namedXmlID[54].strName;
			cell_28 = namedXmlID[55].strName;
			cell_29 = namedXmlID[56].strName;
			cell_30 = namedXmlID[57].strName;
			cell_31 = namedXmlID[58].strName;
			cell_32 = namedXmlID[59].strName;
			cell_33 = namedXmlID[60].strName;
			cell_34 = namedXmlID[61].strName;
			cell_35 = namedXmlID[62].strName;
			cell_36 = namedXmlID[63].strName;
			cell_37 = namedXmlID[64].strName;
			cell_38 = namedXmlID[65].strName;
			cell_39 = namedXmlID[66].strName;
			cell_40 = namedXmlID[67].strName;
			cell_41 = namedXmlID[68].strName;
			cell_42 = namedXmlID[69].strName;
			cell_43 = namedXmlID[70].strName;
			cell_44 = namedXmlID[71].strName;
			cell_45 = namedXmlID[72].strName;
			cell_46 = namedXmlID[73].strName;
			cell_47 = namedXmlID[74].strName;
			cell_48 = namedXmlID[75].strName;
			cell_49 = namedXmlID[76].strName;
			cell_parent = namedXmlID[77].strName;
			chk_auto_comp_promp = namedXmlID[78].strName;
			chk_auto_dot = namedXmlID[79].strName;
			chk_auto_select_cand = namedXmlID[80].strName;
			chk_disable_number_to_select_cand = namedXmlID[81].strName;
			chk_enable_23cand_hotkey = namedXmlID[82].strName;
			chk_full_skip_simple = namedXmlID[83].strName;
			chk_sent_input = namedXmlID[84].strName;
			chk_show_op_tip = namedXmlID[85].strName;
			comp_english = namedXmlID[86].strName;
			comp_lineime = namedXmlID[87].strName;
			comp_normal = namedXmlID[88].strName;
			comp_sent = namedXmlID[89].strName;
			comp_spell = namedXmlID[90].strName;
			comp_start = namedXmlID[91].strName;
			comp_umode = namedXmlID[92].strName;
			config = namedXmlID[93].strName;
			edit_webmode_header = namedXmlID[94].strName;
			enter_for_clear = namedXmlID[95].strName;
			enter_for_input = namedXmlID[96].strName;
			follow_caret = namedXmlID[97].strName;
			forcast_always = namedXmlID[98].strName;
			forcast_disable = namedXmlID[99].strName;
			forcast_no_cand = namedXmlID[100].strName;
			gbk_show_as_gb = namedXmlID[101].strName;
			gbk_show_manual = namedXmlID[102].strName;
			gbk_show_only = namedXmlID[103].strName;
			help = namedXmlID[104].strName;
			hide_statusbar = namedXmlID[105].strName;
			hk_2_cand = namedXmlID[106].strName;
			hk_3_cand = namedXmlID[107].strName;
			hk_bihua_heng = namedXmlID[108].strName;
			hk_bihua_na = namedXmlID[109].strName;
			hk_bihua_pie = namedXmlID[110].strName;
			hk_bihua_shu = namedXmlID[111].strName;
			hk_bihua_wild = namedXmlID[112].strName;
			hk_bihua_zhe = namedXmlID[113].strName;
			hk_filter_gbk = namedXmlID[114].strName;
			hk_input_en = namedXmlID[115].strName;
			hk_make_phrase = namedXmlID[116].strName;
			hk_record = namedXmlID[117].strName;
			hk_show_comp = namedXmlID[118].strName;
			hk_show_statusbar = namedXmlID[119].strName;
			hk_show_table = namedXmlID[120].strName;
			hk_switch_py = namedXmlID[121].strName;
			hk_to_sentmode = namedXmlID[122].strName;
			hk_to_umode = namedXmlID[123].strName;
			hk_tts = namedXmlID[124].strName;
			hk_turn_next = namedXmlID[125].strName;
			hk_turn_prev = namedXmlID[126].strName;
			ime_switch_disabled = namedXmlID[127].strName;
			ime_switch_left_shift = namedXmlID[128].strName;
			ime_switch_right_shift = namedXmlID[129].strName;
			img_logo = namedXmlID[130].strName;
			input_big5 = namedXmlID[131].strName;
			key_map = namedXmlID[132].strName;
			key_speed = namedXmlID[133].strName;
			key_to_sentmode = namedXmlID[134].strName;
			lst_catalog = namedXmlID[135].strName;
			memu_edit_symbol = namedXmlID[136].strName;
			memu_edit_usercmd = namedXmlID[137].strName;
			memu_edit_userdef = namedXmlID[138].strName;
			memu_edit_userjm = namedXmlID[139].strName;
			menu_blend = namedXmlID[140].strName;
			menu_comp = namedXmlID[141].strName;
			menu_data_mgr = namedXmlID[142].strName;
			menu_server = namedXmlID[143].strName;
			menu_skin = namedXmlID[144].strName;
			open_spchar = namedXmlID[145].strName;
			py_switch_disabled = namedXmlID[146].strName;
			py_switch_left_ctrl = namedXmlID[147].strName;
			py_switch_right_ctrl = namedXmlID[148].strName;
			rate_adjust_auto = namedXmlID[149].strName;
			rate_adjust_disable = namedXmlID[150].strName;
			rate_adjust_fast = namedXmlID[151].strName;
			skin_cloud = namedXmlID[152].strName;
			skin_def = namedXmlID[153].strName;
			skin_mgr = namedXmlID[154].strName;
			sound_beep = namedXmlID[155].strName;
			sound_disable = namedXmlID[156].strName;
			sound_wave = namedXmlID[157].strName;
			spell_all = namedXmlID[158].strName;
			spell_one = namedXmlID[159].strName;
			spell_three = namedXmlID[160].strName;
			spell_two = namedXmlID[161].strName;
			status_base = namedXmlID[162].strName;
			status_extend = namedXmlID[163].strName;
			status_shrink = namedXmlID[164].strName;
			stv_sent = namedXmlID[165].strName;
			svr_showicon = namedXmlID[166].strName;
			txt_auto_complete = namedXmlID[167].strName;
			txt_build_time = namedXmlID[168].strName;
			txt_comp = namedXmlID[169].strName;
			txt_comps = namedXmlID[170].strName;
			txt_en_header = namedXmlID[171].strName;
			txt_svr_ver = namedXmlID[172].strName;
			txt_temp_spell_flag = namedXmlID[173].strName;
			txt_tip = namedXmlID[174].strName;
			txt_tip_content = namedXmlID[175].strName;
			txt_tip_title = namedXmlID[176].strName;
			txt_title = namedXmlID[177].strName;
			txt_ver = namedXmlID[178].strName;
			txt_zoomin = namedXmlID[179].strName;
			userdef = namedXmlID[180].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * ass_mode_english_prompt;
		 const wchar_t * ass_mode_none;
		 const wchar_t * ass_mode_phrase;
		 const wchar_t * ass_mode_phrase_prompt;
		 const wchar_t * btn_charmode;
		 const wchar_t * btn_close;
		 const wchar_t * btn_english;
		 const wchar_t * btn_filter_gbk;
		 const wchar_t * btn_help;
		 const wchar_t * btn_make_phrase;
		 const wchar_t * btn_menu;
		 const wchar_t * btn_nextpage;
		 const wchar_t * btn_prevpage;
		 const wchar_t * btn_query;
		 const wchar_t * btn_record;
		 const wchar_t * btn_sound;
		 const wchar_t * btn_status_extend;
		 const wchar_t * btn_status_shrink;
		 const wchar_t * cand_auto_input;
		 const wchar_t * cand_container;
		 const wchar_t * cand_english;
		 const wchar_t * cand_english_input;
		 const wchar_t * cand_normal;
		 const wchar_t * cand_phrase;
		 const wchar_t * cand_py_phrase_first;
		 const wchar_t * cand_tip;
		 const wchar_t * cell_00;
		 const wchar_t * cell_01;
		 const wchar_t * cell_02;
		 const wchar_t * cell_03;
		 const wchar_t * cell_04;
		 const wchar_t * cell_05;
		 const wchar_t * cell_06;
		 const wchar_t * cell_07;
		 const wchar_t * cell_08;
		 const wchar_t * cell_09;
		 const wchar_t * cell_10;
		 const wchar_t * cell_11;
		 const wchar_t * cell_12;
		 const wchar_t * cell_13;
		 const wchar_t * cell_14;
		 const wchar_t * cell_15;
		 const wchar_t * cell_16;
		 const wchar_t * cell_17;
		 const wchar_t * cell_18;
		 const wchar_t * cell_19;
		 const wchar_t * cell_20;
		 const wchar_t * cell_21;
		 const wchar_t * cell_22;
		 const wchar_t * cell_23;
		 const wchar_t * cell_24;
		 const wchar_t * cell_25;
		 const wchar_t * cell_26;
		 const wchar_t * cell_27;
		 const wchar_t * cell_28;
		 const wchar_t * cell_29;
		 const wchar_t * cell_30;
		 const wchar_t * cell_31;
		 const wchar_t * cell_32;
		 const wchar_t * cell_33;
		 const wchar_t * cell_34;
		 const wchar_t * cell_35;
		 const wchar_t * cell_36;
		 const wchar_t * cell_37;
		 const wchar_t * cell_38;
		 const wchar_t * cell_39;
		 const wchar_t * cell_40;
		 const wchar_t * cell_41;
		 const wchar_t * cell_42;
		 const wchar_t * cell_43;
		 const wchar_t * cell_44;
		 const wchar_t * cell_45;
		 const wchar_t * cell_46;
		 const wchar_t * cell_47;
		 const wchar_t * cell_48;
		 const wchar_t * cell_49;
		 const wchar_t * cell_parent;
		 const wchar_t * chk_auto_comp_promp;
		 const wchar_t * chk_auto_dot;
		 const wchar_t * chk_auto_select_cand;
		 const wchar_t * chk_disable_number_to_select_cand;
		 const wchar_t * chk_enable_23cand_hotkey;
		 const wchar_t * chk_full_skip_simple;
		 const wchar_t * chk_sent_input;
		 const wchar_t * chk_show_op_tip;
		 const wchar_t * comp_english;
		 const wchar_t * comp_lineime;
		 const wchar_t * comp_normal;
		 const wchar_t * comp_sent;
		 const wchar_t * comp_spell;
		 const wchar_t * comp_start;
		 const wchar_t * comp_umode;
		 const wchar_t * config;
		 const wchar_t * edit_webmode_header;
		 const wchar_t * enter_for_clear;
		 const wchar_t * enter_for_input;
		 const wchar_t * follow_caret;
		 const wchar_t * forcast_always;
		 const wchar_t * forcast_disable;
		 const wchar_t * forcast_no_cand;
		 const wchar_t * gbk_show_as_gb;
		 const wchar_t * gbk_show_manual;
		 const wchar_t * gbk_show_only;
		 const wchar_t * help;
		 const wchar_t * hide_statusbar;
		 const wchar_t * hk_2_cand;
		 const wchar_t * hk_3_cand;
		 const wchar_t * hk_bihua_heng;
		 const wchar_t * hk_bihua_na;
		 const wchar_t * hk_bihua_pie;
		 const wchar_t * hk_bihua_shu;
		 const wchar_t * hk_bihua_wild;
		 const wchar_t * hk_bihua_zhe;
		 const wchar_t * hk_filter_gbk;
		 const wchar_t * hk_input_en;
		 const wchar_t * hk_make_phrase;
		 const wchar_t * hk_record;
		 const wchar_t * hk_show_comp;
		 const wchar_t * hk_show_statusbar;
		 const wchar_t * hk_show_table;
		 const wchar_t * hk_switch_py;
		 const wchar_t * hk_to_sentmode;
		 const wchar_t * hk_to_umode;
		 const wchar_t * hk_tts;
		 const wchar_t * hk_turn_next;
		 const wchar_t * hk_turn_prev;
		 const wchar_t * ime_switch_disabled;
		 const wchar_t * ime_switch_left_shift;
		 const wchar_t * ime_switch_right_shift;
		 const wchar_t * img_logo;
		 const wchar_t * input_big5;
		 const wchar_t * key_map;
		 const wchar_t * key_speed;
		 const wchar_t * key_to_sentmode;
		 const wchar_t * lst_catalog;
		 const wchar_t * memu_edit_symbol;
		 const wchar_t * memu_edit_usercmd;
		 const wchar_t * memu_edit_userdef;
		 const wchar_t * memu_edit_userjm;
		 const wchar_t * menu_blend;
		 const wchar_t * menu_comp;
		 const wchar_t * menu_data_mgr;
		 const wchar_t * menu_server;
		 const wchar_t * menu_skin;
		 const wchar_t * open_spchar;
		 const wchar_t * py_switch_disabled;
		 const wchar_t * py_switch_left_ctrl;
		 const wchar_t * py_switch_right_ctrl;
		 const wchar_t * rate_adjust_auto;
		 const wchar_t * rate_adjust_disable;
		 const wchar_t * rate_adjust_fast;
		 const wchar_t * skin_cloud;
		 const wchar_t * skin_def;
		 const wchar_t * skin_mgr;
		 const wchar_t * sound_beep;
		 const wchar_t * sound_disable;
		 const wchar_t * sound_wave;
		 const wchar_t * spell_all;
		 const wchar_t * spell_one;
		 const wchar_t * spell_three;
		 const wchar_t * spell_two;
		 const wchar_t * status_base;
		 const wchar_t * status_extend;
		 const wchar_t * status_shrink;
		 const wchar_t * stv_sent;
		 const wchar_t * svr_showicon;
		 const wchar_t * txt_auto_complete;
		 const wchar_t * txt_build_time;
		 const wchar_t * txt_comp;
		 const wchar_t * txt_comps;
		 const wchar_t * txt_en_header;
		 const wchar_t * txt_svr_ver;
		 const wchar_t * txt_temp_spell_flag;
		 const wchar_t * txt_tip;
		 const wchar_t * txt_tip_content;
		 const wchar_t * txt_tip_title;
		 const wchar_t * txt_title;
		 const wchar_t * txt_ver;
		 const wchar_t * txt_zoomin;
		 const wchar_t * userdef;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int ass_mode_english_prompt	=	402;
		const static int ass_mode_none	=	400;
		const static int ass_mode_phrase	=	401;
		const static int ass_mode_phrase_prompt	=	403;
		const static int btn_charmode	=	65593;
		const static int btn_close	=	2;
		const static int btn_english	=	65597;
		const static int btn_filter_gbk	=	65598;
		const static int btn_help	=	65601;
		const static int btn_make_phrase	=	65594;
		const static int btn_menu	=	65600;
		const static int btn_nextpage	=	65580;
		const static int btn_prevpage	=	65578;
		const static int btn_query	=	65599;
		const static int btn_record	=	65595;
		const static int btn_sound	=	65596;
		const static int btn_status_extend	=	65589;
		const static int btn_status_shrink	=	65592;
		const static int cand_auto_input	=	65538;
		const static int cand_container	=	65579;
		const static int cand_english	=	65581;
		const static int cand_english_input	=	65584;
		const static int cand_normal	=	65577;
		const static int cand_phrase	=	65583;
		const static int cand_py_phrase_first	=	65539;
		const static int cand_tip	=	65585;
		const static int cell_00	=	100;
		const static int cell_01	=	101;
		const static int cell_02	=	102;
		const static int cell_03	=	103;
		const static int cell_04	=	104;
		const static int cell_05	=	105;
		const static int cell_06	=	106;
		const static int cell_07	=	107;
		const static int cell_08	=	108;
		const static int cell_09	=	109;
		const static int cell_10	=	110;
		const static int cell_11	=	111;
		const static int cell_12	=	112;
		const static int cell_13	=	113;
		const static int cell_14	=	114;
		const static int cell_15	=	115;
		const static int cell_16	=	116;
		const static int cell_17	=	117;
		const static int cell_18	=	118;
		const static int cell_19	=	119;
		const static int cell_20	=	120;
		const static int cell_21	=	121;
		const static int cell_22	=	122;
		const static int cell_23	=	123;
		const static int cell_24	=	124;
		const static int cell_25	=	125;
		const static int cell_26	=	126;
		const static int cell_27	=	127;
		const static int cell_28	=	128;
		const static int cell_29	=	129;
		const static int cell_30	=	130;
		const static int cell_31	=	131;
		const static int cell_32	=	132;
		const static int cell_33	=	133;
		const static int cell_34	=	134;
		const static int cell_35	=	135;
		const static int cell_36	=	136;
		const static int cell_37	=	137;
		const static int cell_38	=	138;
		const static int cell_39	=	139;
		const static int cell_40	=	140;
		const static int cell_41	=	141;
		const static int cell_42	=	142;
		const static int cell_43	=	143;
		const static int cell_44	=	144;
		const static int cell_45	=	145;
		const static int cell_46	=	146;
		const static int cell_47	=	147;
		const static int cell_48	=	148;
		const static int cell_49	=	149;
		const static int cell_parent	=	65607;
		const static int chk_auto_comp_promp	=	65553;
		const static int chk_auto_dot	=	65551;
		const static int chk_auto_select_cand	=	65552;
		const static int chk_disable_number_to_select_cand	=	65559;
		const static int chk_enable_23cand_hotkey	=	65556;
		const static int chk_full_skip_simple	=	65562;
		const static int chk_sent_input	=	65554;
		const static int chk_show_op_tip	=	65540;
		const static int comp_english	=	65574;
		const static int comp_lineime	=	65573;
		const static int comp_normal	=	65566;
		const static int comp_sent	=	65575;
		const static int comp_spell	=	65569;
		const static int comp_start	=	400;
		const static int comp_umode	=	65571;
		const static int config	=	100;
		const static int edit_webmode_header	=	65555;
		const static int enter_for_clear	=	110;
		const static int enter_for_input	=	111;
		const static int follow_caret	=	102;
		const static int forcast_always	=	601;
		const static int forcast_disable	=	600;
		const static int forcast_no_cand	=	602;
		const static int gbk_show_as_gb	=	702;
		const static int gbk_show_manual	=	701;
		const static int gbk_show_only	=	700;
		const static int help	=	112;
		const static int hide_statusbar	=	101;
		const static int hk_2_cand	=	65557;
		const static int hk_3_cand	=	65558;
		const static int hk_bihua_heng	=	900;
		const static int hk_bihua_na	=	903;
		const static int hk_bihua_pie	=	902;
		const static int hk_bihua_shu	=	901;
		const static int hk_bihua_wild	=	905;
		const static int hk_bihua_zhe	=	904;
		const static int hk_filter_gbk	=	65548;
		const static int hk_input_en	=	65547;
		const static int hk_make_phrase	=	65543;
		const static int hk_record	=	65550;
		const static int hk_show_comp	=	65545;
		const static int hk_show_statusbar	=	65546;
		const static int hk_show_table	=	65544;
		const static int hk_switch_py	=	65541;
		const static int hk_to_sentmode	=	65537;
		const static int hk_to_umode	=	65542;
		const static int hk_tts	=	65549;
		const static int hk_turn_next	=	65561;
		const static int hk_turn_prev	=	65560;
		const static int ime_switch_disabled	=	102;
		const static int ime_switch_left_shift	=	100;
		const static int ime_switch_right_shift	=	101;
		const static int img_logo	=	65587;
		const static int input_big5	=	103;
		const static int key_map	=	110;
		const static int key_speed	=	111;
		const static int key_to_sentmode	=	65536;
		const static int lst_catalog	=	65606;
		const static int memu_edit_symbol	=	702;
		const static int memu_edit_usercmd	=	703;
		const static int memu_edit_userdef	=	701;
		const static int memu_edit_userjm	=	704;
		const static int menu_blend	=	65609;
		const static int menu_comp	=	65608;
		const static int menu_data_mgr	=	65611;
		const static int menu_server	=	65610;
		const static int menu_skin	=	200;
		const static int open_spchar	=	104;
		const static int py_switch_disabled	=	122;
		const static int py_switch_left_ctrl	=	120;
		const static int py_switch_right_ctrl	=	121;
		const static int rate_adjust_auto	=	501;
		const static int rate_adjust_disable	=	500;
		const static int rate_adjust_fast	=	502;
		const static int skin_cloud	=	202;
		const static int skin_def	=	220;
		const static int skin_mgr	=	201;
		const static int sound_beep	=	802;
		const static int sound_disable	=	800;
		const static int sound_wave	=	801;
		const static int spell_all	=	504;
		const static int spell_one	=	501;
		const static int spell_three	=	503;
		const static int spell_two	=	502;
		const static int status_base	=	65591;
		const static int status_extend	=	65590;
		const static int status_shrink	=	65586;
		const static int stv_sent	=	65576;
		const static int svr_showicon	=	601;
		const static int txt_auto_complete	=	65572;
		const static int txt_build_time	=	65565;
		const static int txt_comp	=	65588;
		const static int txt_comps	=	65567;
		const static int txt_en_header	=	65582;
		const static int txt_svr_ver	=	65564;
		const static int txt_temp_spell_flag	=	65570;
		const static int txt_tip	=	65568;
		const static int txt_tip_content	=	65603;
		const static int txt_tip_title	=	65602;
		const static int txt_title	=	65605;
		const static int txt_ver	=	65563;
		const static int txt_zoomin	=	65604;
		const static int userdef	=	505;
		}id;

		class _string{
		public:
		const static int btn_close	=	0;
		const static int cfg_about	=	1;
		const static int cfg_associate	=	2;
		const static int cfg_candidate	=	3;
		const static int cfg_habit	=	4;
		const static int cfg_hotkey	=	5;
		const static int cfg_misc	=	6;
		const static int title_config	=	7;
		}string;

		class _color{
		public:
		const static int associate_english_prefix	=	0;
		const static int blue	=	1;
		const static int cand_comp	=	2;
		const static int cand_first	=	3;
		const static int cand_index	=	4;
		const static int cand_phonetic	=	5;
		const static int cand_text	=	6;
		const static int cand_wild	=	7;
		const static int comp_auto_complete	=	8;
		const static int comp_umode_prefix	=	9;
		const static int configborder	=	10;
		const static int gray	=	11;
		const static int green	=	12;
		const static int red	=	13;
		const static int sent_input	=	14;
		const static int sent_left	=	15;
		const static int sent_right	=	16;
		const static int tabnavbk	=	17;
		const static int temp_spell_flag	=	18;
		const static int tip_cand	=	19;
		const static int tip_comp	=	20;
		const static int white	=	21;
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
