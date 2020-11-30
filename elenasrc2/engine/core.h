//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA Core constants
//
//                                              (C)2020, by Alekey Rakov
//------------------------------------------------------------------------------

#ifndef coreH
#define coreH 1

namespace _ELENA_
{

// --- ELENA Object constants ---
constexpr int gcPageSize32       = 0x0010;            // a heap page size constant
constexpr int elObjectOffset32   = 0x0008;            // object header / offset constant

constexpr int elObjectOffset64   = 0x0008;            // object header / offset constant

constexpr int elPageVMTOffset32  = 0x0008;
constexpr int elPageSizeOffset32 = 0x0004;

constexpr int elVMTCountOffset32 = 0x0004;                // a VMT size offset
constexpr int elVMTClassOffset32 = 0x0010;                // a VMT class offset
constexpr int elVMTFlagOffset32  = 0x000C;                // a VMT class offset

constexpr int elVMTCountOffset64 = 0x0018;                // a VMTX size offset
constexpr int elVMTClassOffset64 = 0x0020;                // a VMTX class offset

} // _ELENA_

#endif // coreH
