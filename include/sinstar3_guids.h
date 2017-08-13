#pragma once

#include <INITGUID.h>

//
// 集中在一起，以便修改，保持数据的一致性
//

// {14437F8E-DAC9-4140-927F-7CE4EEC0D395}
DEFINE_GUID(c_clsidSinstar3TSF, 
			0x14437f8e, 0xdac9, 0x4140, 0x92, 0x7f, 0x7c, 0xe4, 0xee, 0xc0, 0xd3, 0x95);


// {0F70D9CE-F811-4576-A26A-5B691A3251D3}
DEFINE_GUID(c_guidProfile, 
			0xf70d9ce, 0xf811, 0x4576, 0xa2, 0x6a, 0x5b, 0x69, 0x1a, 0x32, 0x51, 0xd3);

//
//  define two guids for display attribute info. This textservice has
//  two display attribute. One is for input text and the other is for the
//  converted text.
//
//      c_guidDisplayAttributeInput 
//      c_guidDisplayAttributeConverted
//

// {75CD50D5-A2BD-4a15-AE83-FAB412416D26}
DEFINE_GUID(c_guidDisplayAttributeInput, 
			0x75cd50d5, 0xa2bd, 0x4a15, 0xae, 0x83, 0xfa, 0xb4, 0x12, 0x41, 0x6d, 0x26);

// {5ECDD5A3-DD23-451a-B996-801845F5F1E2}
DEFINE_GUID(c_guidDisplayAttributeConverted, 
			0x5ecdd5a3, 0xdd23, 0x451a, 0xb9, 0x96, 0x80, 0x18, 0x45, 0xf5, 0xf1, 0xe2);

// {38EA33F3-4245-4da2-82E6-718B223D7C1F}
DEFINE_GUID(c_guidDisplayAttributeTargetConverted, 
			0x38ea33f3, 0x4245, 0x4da2, 0x82, 0xe6, 0x71, 0x8b, 0x22, 0x3d, 0x7c, 0x1f);


