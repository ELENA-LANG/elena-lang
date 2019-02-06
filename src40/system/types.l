/// Attribute categories:
/// visibility? property? scope_prefix? scope? type?

/// visibility:
#80004001 = public;
#80004002 = private;
#80004003 = internal;

/// property:
#80003001 = sealed;
#80003002 = abstract;
#80003003 = closed;
#80003005 = predefined;

/// scope_prefix:
#80002001 = const;
#80002002 = embeddable;
#80002003 = ref;
#80002004 = __direct;
#80002005 = __loop;
#80002006 = preloaded;
#80002007 = get;
#80002008 = set;
#80002009 = lazy;

/// scope:
#80001001 = class;
#80001002 = struct;
#80001003 = symbol;
#80001004 = constructor;
#80001005 = extension;
#80001006 = singleton;
#80001007 = interface;
#80001008 = method;
#80001009 = field;
#8000100A = type;
#8000100B = generic;
#8000100C = closure;
#8000100D = var;
#8000100E = this;
#8000100F = static;
#80001011 = cast;
#80001012 = new;
#80001013 = dispatch;
#80001014 = params;
#80001015 = extern;
#80001016 = intern;
#80001017 = forward;
#80001018 = import;
#80001019 = mixin;
#8000101A = property;
#8000101B = nonstructural;
#8000101C = auto;

/// primitive types
#80000801 = __string;
#80000802 = __float;
#80000803 = __int;
#80000804 = __raw;
#80000805 = __ptr;
#80000806 = __mssg;
#80000807 = __subj;
#80000806 = mssgconst;
#80000807 = subjconst;
#80000808 = __symbol;

//__extmssg       = #21.
//__tape_mixin    = #8209.
//dynamic         = #30.
//stackunsafe     = #16403.

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
type rt_ptr     = RTPointer;
type dirty_ptr  = UnsafePointer;
