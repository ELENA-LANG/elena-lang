/// Attributes
type            = #16390.
class           = #16386.
struct          = #8197.
symbol          = #16389.
constructor     = #16384.
multi           = #16397.
listable        = #8198.
sealed          = #8195.
embeddable      = #8193.
template        = #16391.
extension       = #8203.
stacksafe       = #8192.
explicit        = #16387.
const           = #8201.
singleton       = #8208.
generic         = #8202.
static          = #8194.
dynamic         = #8199.
var             = #16385.
limited         = #8196.
field           = #16392.
method          = #16393.
loop_template   = #16394.
nonstructural   = #8204.
import          = #16395.
extern_template = #16396.
action          = #8205.

args            = #32. 

__string        = #8200.
__int           = #11.
__dword         = #12.
__real          = #14.
__verb          = #20.
__mssg          = #19.
__subj          = #18.
__extmssg       = #21.

__ifoperator    = #4097.
__ifnotoperator = #4098.


//#template(-8207)         preloaded {}
//#template(-4099) w1 {}
//#template(-4100) w2 {}
//#template(-4101) w3 {}
//#8206           mixin.
//#8209           __tape_mixin.
//
//#22 #262144     __symbol.

/// Provides classes that describe the basic functionality

type object     = system'Object.

// === basic logical types ===
type bool       = system'BaseBoolValue.

//// === basic primitive types ===
//
//type keyvalue   :: system'KeyValue.

type ref        = system'Variable.

type byte       = system'ByteNumber.
type short      = system'ShortNumber.
type int        = system'IntNumber.
type uint       = system'UIntNumber.
type long       = system'LongNumber.
type real       = system'RealNumber.
type char       = system'CharValue.
type mssg       = system'Message.
type ext_mssg   = system'ExtensionMessage.

/// A verb symbol

/// An instance of system'Verb class. 
type verb       = system'Verb.

type subject    = system'Signature.
//type symbol_ref :: system'Symbol.

// === basic dynamic types ===
type bytearray  = system'ByteArray.
type shortarray = system'ShortArray.
type intarray   = system'IntArray.
type intmatrix  = system'IntMatrix.
type realarray  = system'RealArray.
type realmatrix = system'RealMatrix.
type chararray  = system'CharArray.
type array      = system'Array.
//#subject mssgarray  = system'MessageArray.
type bitarray   = system'BitArray.
type bitset     = system'BitArray32.

/// An instance of system'LiteralValue class. 
type literal    = system'LiteralValue.

type wide       = system'WideLiteralValue.

// === function types ===
type func       = system'BaseFunction.

/// An anonymous function with a generic parameter.
///
/// An instance of system'BaseFunction1
type func1      = system'BaseFunction1.
type each       = system'BaseFunction1.

//type func2      :: system'BaseFunction2.
//type func3      :: system'BaseFunction3.
//type func4      :: system'BaseFunction4.
//type func5      :: system'BaseFunction5.
//type func6      :: system'BaseFunction6.
//type func7      :: system'BaseFunction7.
//type func8      :: system'BaseFunction8.
//type func9      :: system'BaseFunction9.
//type func10     :: system'BaseFunction10.
//type func11     :: system'BaseFunction11.

type nfunc      = system'IndexFunction1.
type every      = system'IndexFunction1.
//type nnfunc     :: system'IndexFunction2.
//type n3func     :: system'IndexFunction3.

// === system handles ===
type handle     = system'Handle.
//type rt_ptr     :: system'RTPointer.
type dirty_ptr  = system'UnsafePointer.

// --- Enumerators / Indexers ---
type enumerator = system'Enumerator.
type indexer    = system'Indexer.
//type indexer2d  :: system'Indexer2D.

