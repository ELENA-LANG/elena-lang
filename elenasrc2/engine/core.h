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

constexpr int elVMTCountOffset64 = 0x0004;                // a VMTX size offset
constexpr int elVMTClassOffset64 = 0x0018;                // a VMTX class offset

// --- ELENA CORE built-in routines
constexpr int GC_ALLOC = 0x10001;
constexpr int HOOK = 0x10010;
constexpr int INVOKER = 0x10011;
constexpr int INIT_RND = 0x10012;
constexpr int ENDFRAME = 0x10016;
constexpr int CALC_SIZE = 0x1001F;
constexpr int GET_COUNT = 0x10020;
constexpr int THREAD_WAIT = 0x10021;
constexpr int BREAK = 0x10026;
constexpr int EXPAND_HEAP = 0x10028;

constexpr int CORE_GC_TABLE = 0x20002;
constexpr int CORE_STATICROOT = 0x20005;
constexpr int CORE_TLS_INDEX = 0x20007;
constexpr int CORE_THREADTABLE = 0x20008;
constexpr int CORE_MESSAGE_TABLE = 0x2000A;
constexpr int CORE_EH_TABLE = 0x2000B;
constexpr int SYSTEM_ENV = 0x2000C;
constexpr int VOIDOBJ = 0x2000D;
constexpr int VOIDPTR = 0x2000E;

} // _ELENA_

#endif // coreH
