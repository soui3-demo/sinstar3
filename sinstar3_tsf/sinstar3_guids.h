#pragma once

#include <INITGUID.h>

//
// 集中在一起，以便修改，保持数据的一致性
//

// {3A5831D5-489E-4a76-91CA-A69A89ACAF38}
DEFINE_GUID(c_clsidBaidujpTSF, 
			0x3a5831d5, 0x489e, 0x4a76, 0x91, 0xca, 0xa6, 0x9a, 0x89, 0xac, 0xaf, 0x38);

// {BA260BB7-15D6-4c78-9C8B-42C592DD29C5}
DEFINE_GUID(c_guidProfile, 
			0xba260bb7, 0x15d6, 0x4c78, 0x9c, 0x8b, 0x42, 0xc5, 0x92, 0xdd, 0x29, 0xc5);

//
//  define two guids for display attribute info. This textservice has
//  two display attribute. One is for input text and the other is for the
//  converted text.
//
//      c_guidDisplayAttributeInput 
//      c_guidDisplayAttributeConverted
//

// {B75F441E-0447-45c5-91FD-AA6D020F53F1}
DEFINE_GUID(c_guidDisplayAttributeInput, 
			0xb75f441e, 0x447, 0x45c5, 0x91, 0xfd, 0xaa, 0x6d, 0x2, 0xf, 0x53, 0xf1);

// {75FFF246-9B00-4061-96D7-539BE9204283}
DEFINE_GUID(c_guidDisplayAttributeConverted, 
			0x75fff246, 0x9b00, 0x4061, 0x96, 0xd7, 0x53, 0x9b, 0xe9, 0x20, 0x42, 0x83);

// {C25CF92C-A9BF-41dd-BB2E-145FF32178A2}
DEFINE_GUID(c_guidDisplayAttributeTargetConverted, 
			0xc25cf92c, 0xa9bf, 0x41dd, 0xbb, 0x2e, 0x14, 0x5f, 0xf3, 0x21, 0x78, 0xa2);

