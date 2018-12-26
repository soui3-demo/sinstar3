#pragma once

#define FUNID(id) \
enum{FUN_ID=id};\
UINT GetID() {return FUN_ID;}

/////////////////////////////////////////////////////////////////////
template<typename P1>
void toParamStream(CParamStream &  ps, P1 &p1)
{
	ps << p1;
}
template<typename P1>
void fromParamStream(CParamStream &  ps, P1 & p1)
{
	ps >> p1;
}

#define PARAMS1(type,p1) \
void ToStream4##type(CParamStream &  ps){ toParamStream(ps,p1);}\
void FromStream4##type(CParamStream &  ps){fromParamStream(ps,p1);}\

#define TOSTR1(p1)\
std::string toString() const\
{\
	std::stringstream ss; \
	ss <<#p1":"<< p1; \
	return ss.str();\
}

/////////////////////////////////////////////////////////////
template<typename P1, typename P2>
void toParamStream(CParamStream &  ps, P1 &p1, P2 & p2)
{
	ps << p1 << p2;
}
template<typename P1, typename P2>
void fromParamStream(CParamStream &  ps, P1 & p1, P2 &p2)
{
	ps >> p1 >> p2;
}

#define PARAMS2(type,p1,p2) \
void ToStream4##type(CParamStream &  ps){ toParamStream(ps,p1,p2);}\
void FromStream4##type(CParamStream &  ps){fromParamStream(ps,p1,p2);}\


#define TOSTR2(p1,p2)\
std::string toString() const\
{\
	std::stringstream ss; \
	ss <<#p1":"<< p1<<" "#p2":"<<p2; \
	return ss.str();\
}

////////////////////////////////////////////////////////////////////
template<typename P1, typename P2, typename P3>
void toParamStream(CParamStream &  ps, P1 &p1, P2 & p2, P3 & p3)
{
	ps << p1 << p2 << p3;
}
template<typename P1, typename P2, typename P3>
void fromParamStream(CParamStream &  ps, P1 & p1, P2 &p2, P3 & p3)
{
	ps >> p1 >> p2 >> p3;
}

#define PARAMS3(type,p1,p2,p3) \
void ToStream4##type(CParamStream &  ps){ toParamStream(ps,p1,p2,p3);}\
void FromStream4##type(CParamStream &  ps){fromParamStream(ps,p1,p2,p3);}\

#define TOSTR3(p1,p2,p3)\
std::string toString() const\
{\
	std::stringstream ss; \
	ss <<#p1":"<< p1<<" "#p2":"<< p2<<" "#p3":"<< p3; \
	return ss.str();\
}
///////////////////////////////////////////////////////////////////
template<typename P1, typename P2, typename P3, typename P4>
void toParamStream(CParamStream &  ps, P1 &p1, P2 & p2, P3 & p3, P4 & p4)
{
	ps << p1 << p2 << p3<<p4;
}
template<typename P1, typename P2, typename P3, typename P4>
void fromParamStream(CParamStream &  ps, P1 & p1, P2 &p2, P3 & p3, P4 & p4)
{
	ps >> p1 >> p2 >> p3>>p4;
}

#define PARAMS4(type,p1,p2,p3,p4) \
void ToStream4##type(CParamStream &  ps){ toParamStream(ps,p1,p2,p3,p4);}\
void FromStream4##type(CParamStream &  ps){fromParamStream(ps,p1,p2,p3,p4);}\

#define TOSTR4(p1,p2,p3,p4)\
std::string toString() const\
{\
	std::stringstream ss; \
	ss <<#p1":"<< p1<<" "#p2":"<< p2<<" "#p3":"<< p3<<" "#p4":"<< p4; \
	return ss.str();\
}