/// Attributes
#16390 = type;
#16386 = class;
#8197  = struct;
#16389 = symbol;
#16384 = constructor;
//enum            = #8198.
#8195  = sealed;
#8193  = embeddable;
//template        = #16391.
#8203  = extension;

#8201  = const;
#8208  = singleton;
#8202  = generic;
#8194  = static;
#16385 = var;
#50    = auto;
#8196  = interface;
#16392 = property;
#16393 = method;
#16394 = __loop;
#8204  = nonstructural;
#16395 = import;
#16396 = extern;
#8205  = closure;
#8206  = mixin;
#16399 = get;
//__block         = #16400.
//__nested        = #16401.
//preloaded       = #8207.
#16387 = cast;
#16404 = new;
#16405 = forward;
#16406 = set;
#16407 = field;
#16408 = ref;
#16409 = intern;
#16410 = this;

#8210  = abstract;
#8211  = public;
#8212  = private;
#8213  = internal;
#8214  = closed;
#8215  = predefined;
#8216  = dispatch;

#32    = params;

#8200  = __string;

#09    = __float;
#10    = __int;
#11    = __raw;
#12    = __ptr;
//__dword         = #16.
#19    = __mssg;
#18    = __subj;
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

type object     = Object;

// === basic logical types ===
type bool       = BoolValue;

// === basic type aliases ===
type byte       = ByteNumber;
type short      = ShortNumber;
type int        = IntNumber;
type uint       = UIntNumber;
type long       = LongNumber;
type real       = RealNumber;
type char       = CharValue;
type mssg       = Message;
////type ext_mssg   = system'ExtensionMessage.
type string     = String;
type wide       = WideString;
type handle     = Handle;
//type rt_ptr     = system'RTPointer.
type dirty_ptr  = UnsafePointer;
