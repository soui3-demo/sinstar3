//-------------------------------------
//	通用拼音表
//	2007.4.19 hjx 将树形索引修改成有序表二叉索引
//	2006.10.5 hjx
//-------------------------------------

#include "stdafx.h"
#include "spelldata.h"

//声母表

MYSPTEXT g_szTune[SIZE_TUNE]=
{
	{0,""},	//无声母
	{1,"b"},
	{1,"c"},
	{2,"ch"},
	{1,"d"},
	{1,"f"},
	{1,"g"},
	{1,"h"},
	{1,"j"},
	{1,"k"},
	{1,"l"},
	{1,"m"},
	{1,"n"},
	{1,"p"},
	{1,"q"},
	{1,"r"},
	{1,"s"},
	{2,"sh"},
	{1,"t"},
	{1,"w"},
	{1,"x"},
	{1,"y"},
	{1,"z"},
	{2,"zh"},
};

//韵母表
MYSPTEXT g_szRhyme[SIZE_RHYME]=
{
	{1,"a"},
	{2,"ai"},
	{2,"an"},
	{3,"ang"},
	{2,"ao"},
	{1,"e"},
	{2,"ei"},
	{2,"en"},
	{3,"eng"},
	{2,"er"},
	{1,"i"},
	{2,"ia"},
	{3,"ian"},
	{4,"iang"},
	{3,"iao"},
	{2,"ie"},
	{2,"in"},
	{3,"ing"},
	{4,"iong"},
	{2,"iu"},
	{1,"o"},
	{3,"ong"},
	{2,"ou"},
	{1,"u"},
	{2,"ua"},
	{3,"uai"},
	{3,"uan"},
	{4,"uang"},
	{2,"ue"},
	{2,"ui"},
	{2,"un"},
	{2,"uo"},
	{1,"v"},
};

SPELLINFO g_SpellInfo[SIZE_SPELL]=
{//按照拼音排序的拼音信息
	{"a",{0,0}},
	{"ai",{0,1}},
	{"an",{0,2}},
	{"ang",{0,3}},
	{"ao",{0,4}},
	{"ba",{1,0}},
	{"bai",{1,1}},
	{"ban",{1,2}},
	{"bang",{1,3}},
	{"bao",{1,4}},
	{"bei",{1,6}},
	{"ben",{1,7}},
	{"beng",{1,8}},
	{"bi",{1,10}},
	{"bian",{1,12}},
	{"biao",{1,14}},
	{"bie",{1,15}},
	{"bin",{1,16}},
	{"bing",{1,17}},
	{"bo",{1,20}},
	{"bu",{1,23}},
	{"ca",{2,0}},
	{"cai",{2,1}},
	{"can",{2,2}},
	{"cang",{2,3}},
	{"cao",{2,4}},
	{"ce",{2,5}},
	{"cen",{2,7}},
	{"ceng",{2,8}},
	{"cha",{3,0}},
	{"chai",{3,1}},
	{"chan",{3,2}},
	{"chang",{3,3}},
	{"chao",{3,4}},
	{"che",{3,5}},
	{"chen",{3,7}},
	{"cheng",{3,8}},
	{"chi",{3,10}},
	{"chong",{3,21}},
	{"chou",{3,22}},
	{"chu",{3,23}},
	{"chuai",{3,25}},
	{"chuan",{3,26}},
	{"chuang",{3,27}},
	{"chui",{3,29}},
	{"chun",{3,30}},
	{"chuo",{3,31}},
	{"ci",{2,10}},
	{"cong",{2,21}},
	{"cou",{2,22}},
	{"cu",{2,23}},
	{"cuan",{2,26}},
	{"cui",{2,29}},
	{"cun",{2,30}},
	{"cuo",{2,31}},
	{"da",{4,0}},
	{"dai",{4,1}},
	{"dan",{4,2}},
	{"dang",{4,3}},
	{"dao",{4,4}},
	{"de",{4,5}},
	{"dei",{4,6}},
	{"deng",{4,8}},
	{"di",{4,10}},
	{"dia",{4,11}},
	{"dian",{4,12}},
	{"diao",{4,14}},
	{"die",{4,15}},
	{"ding",{4,17}},
	{"diu",{4,19}},
	{"dong",{4,21}},
	{"dou",{4,22}},
	{"du",{4,23}},
	{"duan",{4,26}},
	{"dui",{4,29}},
	{"dun",{4,30}},
	{"duo",{4,31}},
	{"e",{0,5}},
	{"ei",{0,6}},
	{"en",{0,7}},
	{"eng",{0,8}},
	{"er",{0,9}},
	{"fa",{5,0}},
	{"fan",{5,2}},
	{"fang",{5,3}},
	{"fei",{5,6}},
	{"fen",{5,7}},
	{"feng",{5,8}},
	{"fo",{5,20}},
	{"fou",{5,22}},
	{"fu",{5,23}},
	{"ga",{6,0}},
	{"gai",{6,1}},
	{"gan",{6,2}},
	{"gang",{6,3}},
	{"gao",{6,4}},
	{"ge",{6,5}},
	{"gei",{6,6}},
	{"gen",{6,7}},
	{"geng",{6,8}},
	{"gong",{6,21}},
	{"gou",{6,22}},
	{"gu",{6,23}},
	{"gua",{6,24}},
	{"guai",{6,25}},
	{"guan",{6,26}},
	{"guang",{6,27}},
	{"gui",{6,29}},
	{"gun",{6,30}},
	{"guo",{6,31}},
	{"ha",{7,0}},
	{"hai",{7,1}},
	{"han",{7,2}},
	{"hang",{7,3}},
	{"hao",{7,4}},
	{"he",{7,5}},
	{"hei",{7,6}},
	{"hen",{7,7}},
	{"heng",{7,8}},
	{"hong",{7,21}},
	{"hou",{7,22}},
	{"hu",{7,23}},
	{"hua",{7,24}},
	{"huai",{7,25}},
	{"huan",{7,26}},
	{"huang",{7,27}},
	{"hui",{7,29}},
	{"hun",{7,30}},
	{"huo",{7,31}},
	{"ji",{8,10}},
	{"jia",{8,11}},
	{"jian",{8,12}},
	{"jiang",{8,13}},
	{"jiao",{8,14}},
	{"jie",{8,15}},
	{"jin",{8,16}},
	{"jing",{8,17}},
	{"jiong",{8,18}},
	{"jiu",{8,19}},
	{"ju",{8,23}},
	{"juan",{8,26}},
	{"jue",{8,28}},
	{"jun",{8,30}},
	{"ka",{9,0}},
	{"kai",{9,1}},
	{"kan",{9,2}},
	{"kang",{9,3}},
	{"kao",{9,4}},
	{"ke",{9,5}},
	{"ken",{9,7}},
	{"keng",{9,8}},
	{"kong",{9,21}},
	{"kou",{9,22}},
	{"ku",{9,23}},
	{"kua",{9,24}},
	{"kuai",{9,25}},
	{"kuan",{9,26}},
	{"kuang",{9,27}},
	{"kui",{9,29}},
	{"kun",{9,30}},
	{"kuo",{9,31}},
	{"la",{10,0}},
	{"lai",{10,1}},
	{"lan",{10,2}},
	{"lang",{10,3}},
	{"lao",{10,4}},
	{"le",{10,5}},
	{"lei",{10,6}},
	{"leng",{10,8}},
	{"li",{10,10}},
	{"lia",{10,11}},
	{"lian",{10,12}},
	{"liang",{10,13}},
	{"liao",{10,14}},
	{"lie",{10,15}},
	{"lin",{10,16}},
	{"ling",{10,17}},
	{"liu",{10,19}},
	{"lo",{10,20}},
	{"long",{10,21}},
	{"lou",{10,22}},
	{"lu",{10,23}},
	{"luan",{10,26}},
	{"lue",{10,28}},
	{"lun",{10,30}},
	{"luo",{10,31}},
	{"lv",{10,32}},
	{"ma",{11,0}},
	{"mai",{11,1}},
	{"man",{11,2}},
	{"mang",{11,3}},
	{"mao",{11,4}},
	{"me",{11,5}},
	{"mei",{11,6}},
	{"men",{11,7}},
	{"meng",{11,8}},
	{"mi",{11,10}},
	{"mian",{11,12}},
	{"miao",{11,14}},
	{"mie",{11,15}},
	{"min",{11,16}},
	{"ming",{11,17}},
	{"miu",{11,19}},
	{"mo",{11,20}},
	{"mou",{11,22}},
	{"mu",{11,23}},
	{"na",{12,0}},
	{"nai",{12,1}},
	{"nan",{12,2}},
	{"nang",{12,3}},
	{"nao",{12,4}},
	{"ne",{12,5}},
	{"nei",{12,6}},
	{"nen",{12,7}},
	{"neng",{12,8}},
	{"ni",{12,10}},
	{"nian",{12,12}},
	{"niang",{12,13}},
	{"niao",{12,14}},
	{"nie",{12,15}},
	{"nin",{12,16}},
	{"ning",{12,17}},
	{"niu",{12,19}},
	{"nong",{12,21}},
	{"nou",{12,22}},
	{"nu",{12,23}},
	{"nuan",{12,26}},
	{"nue",{12,28}},
	{"nuo",{12,31}},
	{"nv",{12,32}},
	{"o",{0,20}},
	{"ou",{0,22}},
	{"pa",{13,0}},
	{"pai",{13,1}},
	{"pan",{13,2}},
	{"pang",{13,3}},
	{"pao",{13,4}},
	{"pei",{13,6}},
	{"pen",{13,7}},
	{"peng",{13,8}},
	{"pi",{13,10}},
	{"pian",{13,12}},
	{"piao",{13,14}},
	{"pie",{13,15}},
	{"pin",{13,16}},
	{"ping",{13,17}},
	{"po",{13,20}},
	{"pou",{13,22}},
	{"pu",{13,23}},
	{"qi",{14,10}},
	{"qia",{14,11}},
	{"qian",{14,12}},
	{"qiang",{14,13}},
	{"qiao",{14,14}},
	{"qie",{14,15}},
	{"qin",{14,16}},
	{"qing",{14,17}},
	{"qiong",{14,18}},
	{"qiu",{14,19}},
	{"qu",{14,23}},
	{"quan",{14,26}},
	{"que",{14,28}},
	{"qun",{14,30}},
	{"ran",{15,2}},
	{"rang",{15,3}},
	{"rao",{15,4}},
	{"re",{15,5}},
	{"ren",{15,7}},
	{"reng",{15,8}},
	{"ri",{15,10}},
	{"rong",{15,21}},
	{"rou",{15,22}},
	{"ru",{15,23}},
	{"ruan",{15,26}},
	{"rui",{15,29}},
	{"run",{15,30}},
	{"ruo",{15,31}},
	{"sa",{16,0}},
	{"sai",{16,1}},
	{"san",{16,2}},
	{"sang",{16,3}},
	{"sao",{16,4}},
	{"se",{16,5}},
	{"sen",{16,7}},
	{"seng",{16,8}},
	{"sha",{17,0}},
	{"shai",{17,1}},
	{"shan",{17,2}},
	{"shang",{17,3}},
	{"shao",{17,4}},
	{"she",{17,5}},
	{"shei",{17,6}},
	{"shen",{17,7}},
	{"sheng",{17,8}},
	{"shi",{17,10}},
	{"shou",{17,22}},
	{"shu",{17,23}},
	{"shua",{17,24}},
	{"shuai",{17,25}},
	{"shuan",{17,26}},
	{"shuang",{17,27}},
	{"shui",{17,29}},
	{"shun",{17,30}},
	{"shuo",{17,31}},
	{"si",{16,10}},
	{"song",{16,21}},
	{"sou",{16,22}},
	{"su",{16,23}},
	{"suan",{16,26}},
	{"sui",{16,29}},
	{"sun",{16,30}},
	{"suo",{16,31}},
	{"ta",{18,0}},
	{"tai",{18,1}},
	{"tan",{18,2}},
	{"tang",{18,3}},
	{"tao",{18,4}},
	{"te",{18,5}},
	{"tei",{18,6}},
	{"teng",{18,8}},
	{"ti",{18,10}},
	{"tian",{18,12}},
	{"tiao",{18,14}},
	{"tie",{18,15}},
	{"ting",{18,17}},
	{"tong",{18,21}},
	{"tou",{18,22}},
	{"tu",{18,23}},
	{"tuan",{18,26}},
	{"tui",{18,29}},
	{"tun",{18,30}},
	{"tuo",{18,31}},
	{"wa",{19,0}},
	{"wai",{19,1}},
	{"wan",{19,2}},
	{"wang",{19,3}},
	{"wei",{19,6}},
	{"wen",{19,7}},
	{"weng",{19,8}},
	{"wo",{19,20}},
	{"wu",{19,23}},
	{"xi",{20,10}},
	{"xia",{20,11}},
	{"xian",{20,12}},
	{"xiang",{20,13}},
	{"xiao",{20,14}},
	{"xie",{20,15}},
	{"xin",{20,16}},
	{"xing",{20,17}},
	{"xiong",{20,18}},
	{"xiu",{20,19}},
	{"xu",{20,23}},
	{"xuan",{20,26}},
	{"xue",{20,28}},
	{"xun",{20,30}},
	{"ya",{21,0}},
	{"yan",{21,2}},
	{"yang",{21,3}},
	{"yao",{21,4}},
	{"ye",{21,5}},
	{"yi",{21,10}},
	{"yin",{21,16}},
	{"ying",{21,17}},
	{"yo",{21,20}},
	{"yong",{21,21}},
	{"you",{21,22}},
	{"yu",{21,23}},
	{"yuan",{21,26}},
	{"yue",{21,28}},
	{"yun",{21,30}},
	{"za",{22,0}},
	{"zai",{22,1}},
	{"zan",{22,2}},
	{"zang",{22,3}},
	{"zao",{22,4}},
	{"ze",{22,5}},
	{"zei",{22,6}},
	{"zen",{22,7}},
	{"zeng",{22,8}},
	{"zha",{23,0}},
	{"zhai",{23,1}},
	{"zhan",{23,2}},
	{"zhang",{23,3}},
	{"zhao",{23,4}},
	{"zhe",{23,5}},
	{"zhei",{23,6}},
	{"zhen",{23,7}},
	{"zheng",{23,8}},
	{"zhi",{23,10}},
	{"zhong",{23,21}},
	{"zhou",{23,22}},
	{"zhu",{23,23}},
	{"zhua",{23,24}},
	{"zhuai",{23,25}},
	{"zhuan",{23,26}},
	{"zhuang",{23,27}},
	{"zhui",{23,29}},
	{"zhun",{23,30}},
	{"zhuo",{23,31}},
	{"zi",{22,10}},
	{"zong",{22,21}},
	{"zou",{22,22}},
	{"zu",{22,23}},
	{"zuan",{22,26}},
	{"zui",{22,29}},
	{"zun",{22,30}},
	{"zuo",{22,31}},
};

SPELLINDEX g_SpellIndex[SIZE_SPELL]=
{//按照拼音ID排序的拼音索引表
	{{0,0},0},
	{{0,1},1},
	{{0,2},2},
	{{0,3},3},
	{{0,4},4},
	{{0,5},77},
	{{0,6},78},
	{{0,7},79},
	{{0,8},80},
	{{0,9},81},
	{{0,20},230},
	{{0,22},231},
	{{1,0},5},
	{{1,1},6},
	{{1,2},7},
	{{1,3},8},
	{{1,4},9},
	{{1,6},10},
	{{1,7},11},
	{{1,8},12},
	{{1,10},13},
	{{1,12},14},
	{{1,14},15},
	{{1,15},16},
	{{1,16},17},
	{{1,17},18},
	{{1,20},19},
	{{1,23},20},
	{{2,0},21},
	{{2,1},22},
	{{2,2},23},
	{{2,3},24},
	{{2,4},25},
	{{2,5},26},
	{{2,7},27},
	{{2,8},28},
	{{2,10},47},
	{{2,21},48},
	{{2,22},49},
	{{2,23},50},
	{{2,26},51},
	{{2,29},52},
	{{2,30},53},
	{{2,31},54},
	{{3,0},29},
	{{3,1},30},
	{{3,2},31},
	{{3,3},32},
	{{3,4},33},
	{{3,5},34},
	{{3,7},35},
	{{3,8},36},
	{{3,10},37},
	{{3,21},38},
	{{3,22},39},
	{{3,23},40},
	{{3,25},41},
	{{3,26},42},
	{{3,27},43},
	{{3,29},44},
	{{3,30},45},
	{{3,31},46},
	{{4,0},55},
	{{4,1},56},
	{{4,2},57},
	{{4,3},58},
	{{4,4},59},
	{{4,5},60},
	{{4,6},61},
	{{4,8},62},
	{{4,10},63},
	{{4,11},64},
	{{4,12},65},
	{{4,14},66},
	{{4,15},67},
	{{4,17},68},
	{{4,19},69},
	{{4,21},70},
	{{4,22},71},
	{{4,23},72},
	{{4,26},73},
	{{4,29},74},
	{{4,30},75},
	{{4,31},76},
	{{5,0},82},
	{{5,2},83},
	{{5,3},84},
	{{5,6},85},
	{{5,7},86},
	{{5,8},87},
	{{5,20},88},
	{{5,22},89},
	{{5,23},90},
	{{6,0},91},
	{{6,1},92},
	{{6,2},93},
	{{6,3},94},
	{{6,4},95},
	{{6,5},96},
	{{6,6},97},
	{{6,7},98},
	{{6,8},99},
	{{6,21},100},
	{{6,22},101},
	{{6,23},102},
	{{6,24},103},
	{{6,25},104},
	{{6,26},105},
	{{6,27},106},
	{{6,29},107},
	{{6,30},108},
	{{6,31},109},
	{{7,0},110},
	{{7,1},111},
	{{7,2},112},
	{{7,3},113},
	{{7,4},114},
	{{7,5},115},
	{{7,6},116},
	{{7,7},117},
	{{7,8},118},
	{{7,21},119},
	{{7,22},120},
	{{7,23},121},
	{{7,24},122},
	{{7,25},123},
	{{7,26},124},
	{{7,27},125},
	{{7,29},126},
	{{7,30},127},
	{{7,31},128},
	{{8,10},129},
	{{8,11},130},
	{{8,12},131},
	{{8,13},132},
	{{8,14},133},
	{{8,15},134},
	{{8,16},135},
	{{8,17},136},
	{{8,18},137},
	{{8,19},138},
	{{8,23},139},
	{{8,26},140},
	{{8,28},141},
	{{8,30},142},
	{{9,0},143},
	{{9,1},144},
	{{9,2},145},
	{{9,3},146},
	{{9,4},147},
	{{9,5},148},
	{{9,7},149},
	{{9,8},150},
	{{9,21},151},
	{{9,22},152},
	{{9,23},153},
	{{9,24},154},
	{{9,25},155},
	{{9,26},156},
	{{9,27},157},
	{{9,29},158},
	{{9,30},159},
	{{9,31},160},
	{{10,0},161},
	{{10,1},162},
	{{10,2},163},
	{{10,3},164},
	{{10,4},165},
	{{10,5},166},
	{{10,6},167},
	{{10,8},168},
	{{10,10},169},
	{{10,11},170},
	{{10,12},171},
	{{10,13},172},
	{{10,14},173},
	{{10,15},174},
	{{10,16},175},
	{{10,17},176},
	{{10,19},177},
	{{10,20},178},
	{{10,21},179},
	{{10,22},180},
	{{10,23},181},
	{{10,26},182},
	{{10,28},183},
	{{10,30},184},
	{{10,31},185},
	{{10,32},186},
	{{11,0},187},
	{{11,1},188},
	{{11,2},189},
	{{11,3},190},
	{{11,4},191},
	{{11,5},192},
	{{11,6},193},
	{{11,7},194},
	{{11,8},195},
	{{11,10},196},
	{{11,12},197},
	{{11,14},198},
	{{11,15},199},
	{{11,16},200},
	{{11,17},201},
	{{11,19},202},
	{{11,20},203},
	{{11,22},204},
	{{11,23},205},
	{{12,0},206},
	{{12,1},207},
	{{12,2},208},
	{{12,3},209},
	{{12,4},210},
	{{12,5},211},
	{{12,6},212},
	{{12,7},213},
	{{12,8},214},
	{{12,10},215},
	{{12,12},216},
	{{12,13},217},
	{{12,14},218},
	{{12,15},219},
	{{12,16},220},
	{{12,17},221},
	{{12,19},222},
	{{12,21},223},
	{{12,22},224},
	{{12,23},225},
	{{12,26},226},
	{{12,28},227},
	{{12,31},228},
	{{12,32},229},
	{{13,0},232},
	{{13,1},233},
	{{13,2},234},
	{{13,3},235},
	{{13,4},236},
	{{13,6},237},
	{{13,7},238},
	{{13,8},239},
	{{13,10},240},
	{{13,12},241},
	{{13,14},242},
	{{13,15},243},
	{{13,16},244},
	{{13,17},245},
	{{13,20},246},
	{{13,22},247},
	{{13,23},248},
	{{14,10},249},
	{{14,11},250},
	{{14,12},251},
	{{14,13},252},
	{{14,14},253},
	{{14,15},254},
	{{14,16},255},
	{{14,17},256},
	{{14,18},257},
	{{14,19},258},
	{{14,23},259},
	{{14,26},260},
	{{14,28},261},
	{{14,30},262},
	{{15,2},263},
	{{15,3},264},
	{{15,4},265},
	{{15,5},266},
	{{15,7},267},
	{{15,8},268},
	{{15,10},269},
	{{15,21},270},
	{{15,22},271},
	{{15,23},272},
	{{15,26},273},
	{{15,29},274},
	{{15,30},275},
	{{15,31},276},
	{{16,0},277},
	{{16,1},278},
	{{16,2},279},
	{{16,3},280},
	{{16,4},281},
	{{16,5},282},
	{{16,7},283},
	{{16,8},284},
	{{16,10},304},
	{{16,21},305},
	{{16,22},306},
	{{16,23},307},
	{{16,26},308},
	{{16,29},309},
	{{16,30},310},
	{{16,31},311},
	{{17,0},285},
	{{17,1},286},
	{{17,2},287},
	{{17,3},288},
	{{17,4},289},
	{{17,5},290},
	{{17,6},291},
	{{17,7},292},
	{{17,8},293},
	{{17,10},294},
	{{17,22},295},
	{{17,23},296},
	{{17,24},297},
	{{17,25},298},
	{{17,26},299},
	{{17,27},300},
	{{17,29},301},
	{{17,30},302},
	{{17,31},303},
	{{18,0},312},
	{{18,1},313},
	{{18,2},314},
	{{18,3},315},
	{{18,4},316},
	{{18,5},317},
	{{18,6},318},
	{{18,8},319},
	{{18,10},320},
	{{18,12},321},
	{{18,14},322},
	{{18,15},323},
	{{18,17},324},
	{{18,21},325},
	{{18,22},326},
	{{18,23},327},
	{{18,26},328},
	{{18,29},329},
	{{18,30},330},
	{{18,31},331},
	{{19,0},332},
	{{19,1},333},
	{{19,2},334},
	{{19,3},335},
	{{19,6},336},
	{{19,7},337},
	{{19,8},338},
	{{19,20},339},
	{{19,23},340},
	{{20,10},341},
	{{20,11},342},
	{{20,12},343},
	{{20,13},344},
	{{20,14},345},
	{{20,15},346},
	{{20,16},347},
	{{20,17},348},
	{{20,18},349},
	{{20,19},350},
	{{20,23},351},
	{{20,26},352},
	{{20,28},353},
	{{20,30},354},
	{{21,0},355},
	{{21,2},356},
	{{21,3},357},
	{{21,4},358},
	{{21,5},359},
	{{21,10},360},
	{{21,16},361},
	{{21,17},362},
	{{21,20},363},
	{{21,21},364},
	{{21,22},365},
	{{21,23},366},
	{{21,26},367},
	{{21,28},368},
	{{21,30},369},
	{{22,0},370},
	{{22,1},371},
	{{22,2},372},
	{{22,3},373},
	{{22,4},374},
	{{22,5},375},
	{{22,6},376},
	{{22,7},377},
	{{22,8},378},
	{{22,10},399},
	{{22,21},400},
	{{22,22},401},
	{{22,23},402},
	{{22,26},403},
	{{22,29},404},
	{{22,30},405},
	{{22,31},406},
	{{23,0},379},
	{{23,1},380},
	{{23,2},381},
	{{23,3},382},
	{{23,4},383},
	{{23,5},384},
	{{23,6},385},
	{{23,7},386},
	{{23,8},387},
	{{23,10},388},
	{{23,21},389},
	{{23,22},390},
	{{23,23},391},
	{{23,24},392},
	{{23,25},393},
	{{23,26},394},
	{{23,27},395},
	{{23,29},396},
	{{23,30},397},
	{{23,31},398},
};

CSpellData g_SpellData;

//按拼音排序的拼音信息比较接口
//lParam:flag(part match)+psp1的长度
int SpInfoCmp(SPELLINFO *pspi1,SPELLINFO *pspi2,LPARAM lParam)
{
	int nRet=0;
	char *psp1=(char *)pspi1;
	char *psp2=pspi2->szSpell;
	char i=0;
	WORD wPart=HIWORD(lParam);
	WORD wLen=LOWORD(lParam);
	while(i<wLen && psp2[i])
	{
		nRet=psp1[i]-psp2[i];
		if(nRet!=0) break;
		i++;
	}
	if(nRet==0)
	{
		if(!wPart)
		{//完全匹配
			if(i<wLen)
				nRet=1;
			else if(psp2[i])
				nRet=-1;
		}else
		{//部分匹配,wLen可以小于pspi2->len
			if(i<wLen)
				nRet=1;
		}
	}
	return nRet;
}

//按拼音ID排序的拼音信息的声母比较接口
int SpIndexCmp(SPELLINDEX *pspi1,SPELLINDEX *pspi2,LPARAM lParam)
{
	int nRet=0;
	LPBYTE pbyID1=(LPBYTE)pspi1;
	char i=0;
	while(i<lParam)
	{
		nRet=pbyID1[i]-pspi2->byID[i];
		if(nRet!=0) break;
		i++;
	}
	return nRet;
}

int MYSPTEXTCmp(MYSPTEXT *pspt1,MYSPTEXT *pspt2,LPARAM lParam)
{
	int nRet=0;
	char i=0;
	char *pszText=(char*)pspt1;
	while(i<lParam && i<pspt2->cLen)
	{
		nRet=pszText[i]-pspt2->szText[i];
		if(nRet!=0) break;
		i++;
	}
	if(nRet==0)
	{
		if(i<lParam)
			nRet=1;
		else if(i<pspt2->cLen)
			nRet=-1;
	}
	return nRet;
}

CSpellData::CSpellData()
{
	m_arrSpInfo.Attach(g_SpellInfo,SIZE_SPELL);
	m_arrSpIndex.Attach(g_SpellIndex,SIZE_SPELL);
	m_arrSpTune.Attach(g_szTune,SIZE_TUNE);
	m_arrSpRhyme.Attach(g_szRhyme,SIZE_RHYME);
}

CSpellData::~CSpellData()
{
	m_arrSpInfo.Detach(NULL);
	m_arrSpIndex.Detach(NULL);
	m_arrSpTune.Detach(NULL);
	m_arrSpRhyme.Detach(NULL);
}


//将拼音转换成ID模式
BOOL CSpellData::Spell2ID(LPCSTR pszSpell, int nLen, BYTE byID[])
{
	if(nLen==-1) nLen=strlen(pszSpell);
	if(nLen>6) return FALSE;
	int nIndex=m_arrSpInfo.SortFind((SPELLINFO*)pszSpell,SpInfoCmp,nLen,0,SFT_ANY);
	if(nIndex==-1) return FALSE;
	memcpy(byID,m_arrSpInfo[nIndex].byID,2);
	return TRUE;
}

BOOL CSpellData::Spell2ID(LPCWSTR pszSpell,int nLen,BYTE byID[2])
{
	if(nLen>7) return FALSE;
	char szSpell[7];
	ASSERT(nLen<=7);
	if(!CUtils::CopyCompFormWStr2AStr(szSpell,pszSpell,nLen))
		return FALSE;
	return Spell2ID(szSpell,nLen,byID);
}

//探测是否还存在更长的拼音
BOOL CSpellData::DetectSpell(LPCSTR pszSpell,int nLen)
{
	if(nLen==-1) nLen=strlen(pszSpell);
	if(nLen>=6) return FALSE;
	int nIndex=m_arrSpInfo.SortFind((SPELLINFO*)pszSpell,SpInfoCmp,nLen,0,SFT_HEAD);
	if(nIndex==-1) return FALSE;
	while(nIndex<SIZE_SPELL)
	{
		if(memcmp(g_SpellInfo[nIndex].szSpell,pszSpell,nLen)!=0) break;
		if(g_SpellInfo[nIndex].szSpell[nLen]!=0) return TRUE;
		nIndex++;
	}
	return FALSE;
}

//获取一个不完整的拼音的ID，只取第一个匹配的拼音
//return:补充的拼音字母数量,-1:非法拼音头
int CSpellData::Spell2IDEx(LPCSTR pszSpell, int nLen, BYTE byID[])
{
	if(nLen==-1) nLen=strlen(pszSpell);
	if(nLen>6) return -1;
	//step1:检查是不是一个纯声母
	if(nLen<3 && GetTuneID(pszSpell,nLen,byID))
	{
		byID[1]=0xFF;
		return 0;
	}
	//step2:检查是不是一个合法的拼音
	int nIndex=m_arrSpInfo.SortFind((SPELLINFO*)pszSpell,SpInfoCmp,MAKELONG(nLen,1),0,SFT_HEAD);
	if(nIndex!=-1)
	{
		memcpy(byID,g_SpellInfo[nIndex].byID,2);
		return strlen(g_SpellInfo[nIndex].szSpell)-nLen;
	}else
	{
		return -1;
	}
}


BOOL CSpellData::ID2Spell(BYTE byID[], char szSpell[])
{
	if(byID[0]>=SIZE_TUNE || byID[1]>=SIZE_RHYME) return FALSE;
	sprintf(szSpell,"%s%s",g_szTune[byID[0]].szText,g_szRhyme[byID[1]].szText);
	return TRUE;
}

BOOL CSpellData::GetTuneID(LPCSTR pszTune, int nLen, BYTE *pbyTuneID)
{
	if(nLen==-1) nLen=strlen(pszTune);
	int nIndex=m_arrSpTune.SortFind((MYSPTEXT*)pszTune,MYSPTEXTCmp,nLen,0,SFT_ANY);
	if(nIndex==-1) return FALSE;
	*pbyTuneID=(BYTE)nIndex;
	return TRUE;
}

BOOL CSpellData::GetTuneID(LPCWSTR pszTune,int nLen,BYTE *pbyTundID)
{
	if(nLen>7) return FALSE;
	char szTune[7];
	if(!CUtils::CopyCompFormWStr2AStr(szTune,pszTune,nLen))
		return FALSE;
	return GetTuneID(szTune,nLen,pbyTundID);
}

int CSpellData::SplitSpell2Rhymes(LPCSTR pszSpell, int nLen, int nSpells[MAX_SENTLEN / 2],bool bTestJP)
{
	if(bTestJP)
	{
		bool bJP = true;
		for(int i=0;i<nLen;i++)
		{
			BYTE byID[2];
			if(Spell2IDEx(pszSpell+i,1,byID)==-1)
			{
				bJP=false;
				break;
			}
			nSpells[i]=1;
		}
		if(bJP)
		{
			return nLen;
		}
	}
	int nRet = 0;
	int i = 0;
	while (i < nLen)
	{
		if (nRet == MAX_SENTLEN / 2)
		{//avoid buffer overflow.
			nRet = 0;
			break;
		}
		BYTE byID[2];
		int nRhymeLen = ExtractSpell(pszSpell+i, nLen-i, byID);
		if (nRhymeLen == 0)
		{//not valid spell string.
			if(nRet>0 && nSpells[nRet-1]>1)
			{//check for previous rhyme, and try to borrow the last alphabet.
				if (-1==Spell2IDEx(pszSpell+i-nSpells[nRet-1], nSpells[nRet-1]-1, byID))
				{//can't borrow.
					nRet =0;
					break;
				}
				//borrow succeed
				nSpells[nRet-1]--;
				i--;
				nRhymeLen = ExtractSpell(pszSpell+i, nLen-i, byID);
				if (nRhymeLen == 0)
				{
					nRet = 0;
					break;
				}
			}else
			{
				nRet = 0;
				break;
			}
		}
		nSpells[nRet++] = nRhymeLen;
		i += nRhymeLen;
	}
	return nRet;
}

int CSpellData::ExtractSpell(LPCSTR pszSpell, int nLen,BYTE byID[2])
{
	if (nLen == -1) nLen = strlen(pszSpell);
	assert(nLen > 0);
	int nValidLen = 0;
	for (int i = 1; i < 7 && i <= nLen; i++)
	{
		if (-1==Spell2IDEx(pszSpell, i, byID))
			break;
		nValidLen = i;
	}
	return nValidLen;
}

BOOL CSpellData::GetRhymeID(LPCSTR pszRhyme, int nLen, BYTE *pbyRhymeID)
{
	if(nLen==-1) nLen=strlen(pszRhyme);
	int nIndex=m_arrSpRhyme.SortFind((MYSPTEXT*)pszRhyme,MYSPTEXTCmp,nLen,0,SFT_ANY);
	if(nIndex==-1) return FALSE;
	*pbyRhymeID=(BYTE)nIndex;
	return TRUE;
}

BOOL CSpellData::GetRhymeID(LPCWSTR pszRhyme,int nLen,BYTE *pbyRhymeID)
{
	if(nLen>7) return FALSE;
	char szRhyme[7];
	if(!CUtils::CopyCompFormWStr2AStr(szRhyme,pszRhyme,nLen))
		return FALSE;
	return GetTuneID(szRhyme,nLen,pbyRhymeID);

}

short CSpellData::GetIDIndex(const BYTE byID[])
{
	int nLen=byID[1]==0xFF?1:2;
	int nIndex=m_arrSpIndex.SortFind((SPELLINDEX*)byID,SpIndexCmp,nLen,0,SFT_ANY);
	ASSERT(nIndex!=-1);
	return g_SpellIndex[nIndex].sIndex;
}

//根据声母找全与之相匹配的韵母,最大不会超过SIZE_RHYME
BYTE CSpellData::MakeupSpell(BYTE byTune, BYTE byRhyme[SIZE_RHYME])
{
	BYTE byRet=0;
	int nBegin=m_arrSpIndex.SortFind((SPELLINDEX*)&byTune,SpIndexCmp,1,0,SFT_HEAD);
	int nEnd=m_arrSpIndex.SortFind((SPELLINDEX*)&byTune,SpIndexCmp,1,0,SFT_TAIL);
	ASSERT(nBegin!=-1 && nEnd!=-1);
	for(int i=nBegin;i<=nEnd;i++)
	{
		byRhyme[byRet++]=g_SpellIndex[i].byID[1];
	}
	return byRet;
}

//判断一个拼音是否合法
BOOL CSpellData::IsSpellValid(BYTE byID[])
{
	int nIndex=m_arrSpIndex.SortFind((SPELLINDEX*)byID,SpIndexCmp,2,0,SFT_ANY);
	return nIndex!=-1;
}

//获取等价的韵母.
int CSpellData::GuessRhymeIDs(const BYTE byID[2],BYTE outIDs[][2],int nLen)
{
	ASSERT(byID[0]==0);
	if(byID[0]!=0)
		return 0;
	int idx=m_arrSpIndex.SortFind((SPELLINDEX*)byID,SpIndexCmp,2,0,SFT_ANY);
	if(idx == -1) return 0;
	MYSPTEXT *test = g_szRhyme+byID[1];
	int ret = 0;
	for(int i=idx+1;i<m_arrSpIndex.GetSize() && ret <nLen;i++)
	{
		SPELLINDEX *pSpIdx = m_arrSpIndex.GetPtAt(i);
		if(pSpIdx->byID[0]!=0) break;
		int iRhyme = pSpIdx->byID[1];
		if(g_szRhyme[iRhyme].cLen<=test->cLen || strncmp(test->szText,g_szRhyme[iRhyme].szText,test->cLen)!=0)
			break;
		memcpy(outIDs[ret++],pSpIdx->byID,2);
	}
	return ret;
}



