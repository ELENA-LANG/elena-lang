/// Attribute categories:
/// visibility? property? scope_prefix? scope? type?

/// visibility:
#define public      80005001h
#define private     80005002h
#define internal    80005003h

/// property:
#define sealed      80004001h
#define abstract    80004002h
//#define closed     80004003h
#define predefined  80004005h

/// scope_accessors:
#define get         80003007h
//#80003008 = set;

/// scope_prefix:

#define const       80002001h
//#80002002 = embeddable;
#define embeddable  80002002h
#define ref         80002003h
//#80002004 = __direct;
#define __loop      80002005h
//#80002006 = preloaded;
//#80002009 = lazy;
//
///// scope:
//#80001001 = class;
//#80001002 = struct;
//#80001003 = symbol;
//#80001004 = constructor;
//#80001005 = extension;
//#80001006 = singleton;
//#80001007 = interface;
//#80001008 = method;
//#80001009 = field;
//#8000100A = type;
//#8000100B = generic;
//#8000100C = closure;
//#8000100D = var;
//#8000100E = this;
//#8000100F = static;
#define static      8000100Fh
//#80001011 = cast;
#define new         80001012h
//#80001013 = dispatch;
#define params      80001014h
//#80001015 = extern;
//#80001016 = intern;
//#80001017 = forward;
//#80001018 = import;
//#80001019 = mixin;
#define property    8000101Ah
//#8000101B = nonstructural;
//#8000101C = auto;
#define attribute   8000101Fh

/// primitive types
#define __string    80000801h
//#80000802 = __float;
//#80000803 = __int;
//#80000804 = __raw;
//#80000805 = __ptr;
//#80000806 = __mssg;
//#80000807 = __subj;
//#80000806 = mssgconst;
//#80000807 = subjconst;
//#80000808 = __symbol;
//
////__extmssg       = #21.
////__tape_mixin    = #8209.
////dynamic         = #30.
////stackunsafe     = #16403.
//
///// Provides classes that describe the basic functionality
//
//type object     = Object;
//
//// === basic logical types ===
//type bool       = BoolValue;
//
//// === basic type aliases ===
//type byte       = ByteNumber;
//type short      = ShortNumber;
//type int        = IntNumber;
//type uint       = UIntNumber;
//type long       = LongNumber;
//type real       = RealNumber;
//type char       = CharValue;
//type mssg       = Message;
//////type ext_mssg   = system'ExtensionMessage.
//type string     = String;
//type wide       = WideString;
//type handle     = Handle;
//type rt_ptr     = RTPointer;
//type dirty_ptr  = UnsafePointer;
