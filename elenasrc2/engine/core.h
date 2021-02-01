//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA Core constants
//
//                                              (C)2020-2021, by Aleksey Rakov
//------------------------------------------------------------------------------

#ifndef coreH
#define coreH 1

namespace _ELENA_
{

// --- 32bit ELENA Object constants ---
constexpr int gcPageSize32             = 0x0010;            // a heap page size constant
constexpr int gcPageSizeOrder32        = 4;
constexpr int gcPageSizeOrderMinus2_32 = 2;
constexpr int gcPageCeil32             = 0x17;
constexpr int gcPageMask32             = 0x0FFFFFFF0;
constexpr int elObjectOffset32         = 0x0008;            // object header / offset constant

constexpr int elPageVMTOffset32        = 0x0008;
constexpr int elPageSizeOffset32       = 0x0004;

constexpr int elVMTCountOffset32       = 0x0004;                // a VMT size offset
constexpr int elVMTClassOffset32       = 0x0010;                // a VMT class offset
constexpr int elVMTFlagOffset32        = 0x000C;                // a VMT class offset

constexpr int elStructMask32           = 0x800000;
constexpr int elSizeCeil32             = 0x0FFFFC;
constexpr int elObjectSizeMask32       = 0xFFFFFF;

// --- 64bit ELENA Object constants ---
constexpr int gcPageSize64             = 0x0020;            // a heap page size constant
constexpr int gcPageSizeOrder64        = 5;
constexpr int gcPageCeil64             = 0x2F;
constexpr int gcPageMask64             = 0x0FFFFFFE0;
constexpr int gcPageSizeOrderMinus2_64 = 3;
constexpr int elObjectOffset64         = 0x0010;            // object header / offset constant

constexpr int elPageVMTOffset64        = 0x0010;
constexpr int elPageSizeOffset64       = 0x0004;

constexpr int elVMTCountOffset64       = 0x0008;                // a VMTX size offset
constexpr int elVMTClassOffset64       = 0x0020;                // a VMTX class offset
constexpr int elVMTFlagOffset64        = 0x0018;                // a VMTX flags offset

constexpr int elStructMask64           = 0x40000000;
constexpr int elSizeCeil64             = 0x3FFFFFF8;
constexpr int elObjectSizeMask64       = 0x7FFFFFFF;

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
constexpr int GC_ALLOCPERM = 0x10031;

constexpr int CORE_GC_TABLE = 0x20002;
constexpr int CORE_STATICROOT = 0x20005;
constexpr int CORE_TLS_INDEX = 0x20007;
constexpr int CORE_THREADTABLE = 0x20008;
constexpr int CORE_MESSAGE_TABLE = 0x2000A;
constexpr int CORE_EH_TABLE = 0x2000B;
constexpr int SYSTEM_ENV = 0x2000C;
constexpr int VOIDOBJ = 0x2000D;
constexpr int VOIDPTR = 0x2000E;

struct ObjectPage32
{
   uintptr_t vmtPtr;
   int       size;
   int       body[2];
};

struct ObjectPage64
{
   uintptr_t vmtPtr;
   int       lock_flag;
   int       size;
   int       body[4];
};

} // _ELENA_

#endif // coreH
