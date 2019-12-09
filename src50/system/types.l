/// Attribute categories:
/// modificator? visibility? property? scope_prefix? scope? type?

/// modificator:
//#define __ignore_duplicates 80006001h 

/// visibility:
#define public      80005001h
#define private     80005002h
#define internal    80005003h
////#define __meta      80005004h
//#define inline      80005005h
//#define property    80005006h

/// property:
#define sealed      80004001h
#define abstract    80004002h
#define closed      80004003h
//#define predefined  80004005h
//#define yieldable   80004006h
//
///// scope_accessors:
//#define get         80003007h
//#define set         80003008h

/// scope_prefix:

#define const       80002001h
#define embeddable  80002002h
#define ref         80002003h
////#80002004 = __direct;
//#define __loop      80002005h
//#define preloaded   80002006h
//#define lazy        80002009h

/// scope:
#define class       80001001h
#define struct      80001002h
#define symbol      80001003h
#define constructor 80001004h
#define extension   80001005h
#define singleton   80001006h
#define interface   80001007h
#define method      80001008h
#define field       80001009h
#define type        8000100Ah
//#define generic     8000100Bh
#define function    8000100Ch
#define var         8000100Dh
#define this        8000100Eh
#define static      8000100Fh
#define cast        80001011h
#define new         80001012h
#define dispatch    80001013h
//#define params      80001014h
//#define extern      80001015h
#define intern      80001016h
//#define forward     80001017h
#define import      80001018h
//#define mixin       80001019h
////#8000101B = nonstructural;
#define auto        8000101Ch
//#define attribute   8000101Fh
//#define yield       80001020h
#define namespace   80001021h

/// primitive types
#define __string    80000801h
//#define __float     80000802h
#define __int       80000803h
//#define __raw       80000804h
//#define __ptr       80000805h
//#define __mssg      80000806h
//#define __subj      80000807h
//#define mssgconst   80000806h
//#define subjconst   80000807h
//#define __symbol    80000808h
//#define inlinearg   80000809h
//
////__extmssg       = #21.
////__tape_mixin    = #8209.
////dynamic         = #30.
////stackunsafe     = #16403.

/// Provides classes that describe the basic functionality

type object     = system'Object;

// === basic logical types ===
type bool       = system'BoolValue;

// === basic type aliases ===
type byte       = system'ByteNumber;
type short      = system'ShortNumber;
type int        = system'IntNumber;
//type uint       = UIntNumber;
//type long       = LongNumber;
//type real       = RealNumber;
//type char       = CharValue;
//type mssg       = Message;
////type ext_mssg   = system'ExtensionMessage.
type string     = system'String;
//type wide       = WideString;
//type handle     = Handle;
//type rt_ptr     = RTPointer;
//type dirty_ptr  = UnsafePointer;
