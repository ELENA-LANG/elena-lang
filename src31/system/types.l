/// Attributes
#16390          type.
#16395          import.
#16391          template.
#16391 #16394   loop_template.
#16391 #16396   extern_template.
#16385          var.
#16386          class.
#16384          constructor.
#16387 #8195    implicit.
#16389          symbol.
#16393          method.
#16392          field.

#8194           static.
//#template(-8207)         preloaded {}
#8202 #8195     generic.
#8192           stacksafe.
#8205           action.
//#template(-4099) w1 {}
//#template(-4100) w2 {}
//#template(-4101) w3 {}
#8195           sealed.
#8199           dynamic.
#8196           limited.
#8206           mixin.
#8209           __tape_mixin.
#8201           const.
#8197           struct.
#8204           nonstructural.
#8198           listable.
#8200           string.
#8193           embeddable.
#8203           extension.
#8208           singleton.

#4097           __ifoperator.
#4098           __ifnotoperator.
#19 #262144     __mssg .
#20 #262144     __verb.
#22 #262144     __symbol.
#21 #524288     __extmssg.
#18 #262144     __subj.
#11 #65536      __int8.
#11 #131072     __int16.
#11 #262144     __int32.
#12 #262144     __dword.
#12 #262144     __ptr.
#13 #524288     __int64.
#14 #524288     __real64.

/// Provides classes that describe the basic functionality

// === basic logical types ===
type bool    :: system'BaseBoolValue.

// === basic primitive types ===

type keyvalue   :: system'KeyValue.

type ref        :: system'Variable.

type byte       :: system'ByteNumber.
type short      :: system'ShortNumber.
type int        :: system'IntNumber.
type long       :: system'LongNumber.
type real       :: system'RealNumber.
type char       :: system'CharValue.
type mssg       :: system'Message.
type ext_mssg   :: system'ExtensionMessage.

/// A verb symbol

/// An instance of system'Verb class. 
type verb       :: system'Verb.

type subject    :: system'Signature.
type symbol_ref :: system'Symbol.

// === basic dynamic types ===
type bytearray  :: system'ByteArray.
type shortarray :: system'ShortArray.
type intarray   :: system'IntArray.
type intmatrix  :: system'IntMatrix.
type realarray  :: system'RealArray.
type realmatrix :: system'RealMatrix.
type chararray  :: system'CharArray.
type array      :: system'Array.
//#subject mssgarray  = system'MessageArray.
type bitarray   :: system'BitArray.
type bitset     :: system'BitArray32.

/// An instance of system'LiteralValue class. 
type literal    :: system'LiteralValue.

type wide       :: system'WideLiteralValue.

// === function types ===
type func       :: system'BaseFunction.

/// An anonymous function with a generic parameter.
///
/// An instance of system'BaseFunction1
type func1      :: system'BaseFunction1.

type func2      :: system'BaseFunction2.
type func3      :: system'BaseFunction3.
type func4      :: system'BaseFunction4.
type func5      :: system'BaseFunction5.
type func6      :: system'BaseFunction6.
type func7      :: system'BaseFunction7.
type func8      :: system'BaseFunction8.
type func9      :: system'BaseFunction9.
type func10     :: system'BaseFunction10.
type func11     :: system'BaseFunction11.

type nfunc      :: system'IndexFunction1.
type nnfunc     :: system'IndexFunction2.
type n3func     :: system'IndexFunction3.

// === system handles ===
type handle     :: system'Handle.
type rt_ptr     :: system'RTPointer.
type dirty_ptr  :: system'UnsafePointer.

// --- Enumerators / Indexers ---
type enumerator :: system'Enumerator.
type indexer    :: system'Indexer.
type indexer2d  :: system'Indexer2D.

// --- Generic attributes ---
type object.

// --- Virtual subjects ---
type args.
