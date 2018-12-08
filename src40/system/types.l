/// Attributes
//V               = #30.
//
#16390 = type;
#16386 = class;
#8197  = struct;
#16389 = symbol;
#16384 = constructor;
//enum            = #8198.
#8195  = sealed;
#8193  = embeddable;
//template        = #16391.
//extension       = #8203.
//
#8201  = const;
#8208  = singleton;
//generic         = #8202.
#8194  = static;
#16385 = var;
//auto            = #50.
//#8196  = interface;
//field           = #16392.
#16393 = method;
//__loop          = #16394.
////nonstructural   = #8204.
//import          = #16395.
//__extern        = #16396.
//closure         = #8205.
//mixin           = #8206.
//__accessor      = #16399.
//__block         = #16400.
//__nested        = #16401.
//preloaded       = #8207.
#16387 = cast;
#16404 = new;
//set             = #16402.  
//
//abstract        = #8210. 
#8211  = public;
//private         = #8212.
#8213  = internal;
#8214  = closed;
//predefined      = #8215.
#8216  = dispatch;
#16405 = forward;

//__args          = #32. 
//
//__string        = #8200.
#11    = __raw;
//__ptr           = #12.
//__real          = #15.
//__dword         = #16.
//__mssg          = #19.
//__subj          = #18.
//__extmssg       = #21.
////__tape_mixin    = #8209.
//__symbol        = #22.
//dynamic         = #30.
//stackunsafe     = #16403.
//
//__ifoperator    = #4097.
//__ifnotoperator = #4098.
//
/// Provides classes that describe the basic functionality

type object     = system'Object;

//// === basic logical types ===
//type bool       = system'BoolValue.

// === basic type aliases ===
type byte       = system'ByteNumber;
type short      = system'ShortNumber;
type int        = system'IntNumber;
//type uint       = system'UIntNumber.
//type long       = system'LongNumber.
//type real       = system'RealNumber.
//type char       = system'CharValue.
//type mssg       = system'Message.
////type ext_mssg   = system'ExtensionMessage.
type literal    = system'LiteralValue;
//type wide       = system'WideLiteralValue.
//type handle     = system'Handle.
//type rt_ptr     = system'RTPointer.
//type dirty_ptr  = system'UnsafePointer.
