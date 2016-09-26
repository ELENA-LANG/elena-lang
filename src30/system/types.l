///// Field attributes
//#ftemplate(-1,1) int8 {}
//#ftemplate(-1,2) int16 {}
//#ftemplate(-1,4) int32 {}
//#ftemplate(-2,8) int64 {}
//#ftemplate(-4,8) float64 {}
//#ftemplate(-7,4) symbol {}
//#ftemplate(-8,4) signature {}
//#ftemplate(-9,4) message {}
//#ftemplate(-10,8) extension_message {}
#template(-2)  static {}
//#ftemplate(-33)  const {}
//#ftemplate(-34)  preloaded {}
//
///// Method attributes
//#mtemplate(40h) generic {}
//#mtemplate(10h) stacksafe {}
//#mtemplate(20h) embeddable {}
//#mtemplate(80h) action {}
//#mtemplate(01h) sealed {}
//
//#mtemplate(-2) w2 {}
//#mtemplate(-3) w3 {}

/// class attributes
#template(0120h) sealed  {}
//#template(0004h) dynamic {}
#template(0020h) limited {}
//#template(0200h) group   {}
//#template(0800h) const   {}
//#template(0008h) struct  {}
//#template(1000h) nonstructural {}
#template(100020h,0080h) enumlist {}
//
//#template(0120h,4980h)         extension {}
//#template(0120h,4980h,subject) extension : subject {}
//
///// Class data attributes
//#template(30000h,8h,4h)  string {}
//#template(0008h,0010h)   embeddable {}
//#template(0008h,800010h) reference {}
//
///// Provides classes that describe the basic functionality
//
//// === basic logical types ===
//#subject bool       = system'BaseBoolValue.
//
//// === basic primitive types ===
//
//#subject keyvalue   = system'KeyValue.
//
//#subject ref        = system'Variable.
//
//#subject byte       = system'ByteNumber.
//#subject short      = system'ShortNumber.
//#subject int        = system'IntNumber.
//#subject long       = system'LongNumber.
//#subject real       = system'RealNumber.
//#subject char       = system'CharValue.
//#subject mssg       = system'Message.
//#subject ext_mssg   = system'ExtensionMessage.
//
///// A verb symbol
/////
///// An instance of system'Verb class. 
//#subject verb       = system'Verb.
//
//#subject subject    = system'Signature.
//#subject symbol_ref = system'Symbol.
//
//// === basic dynamic types ===
//#subject bytearray  = system'ByteArray.
//#subject shortarray = system'ShortArray.
//#subject intarray   = system'IntArray.
//#subject intmatrix  = system'IntMatrix.
//#subject realarray  = system'RealArray.
//#subject realmatrix = system'RealMatrix.
//#subject chararray  = system'CharArray.
//#subject array      = system'Array.
//#subject args       = system'OpenArgs.
//#subject mssgarray  = system'MessageArray.
//#subject bitarray   = system'BitArray.
//#subject bitset     = system'BitArray32.
//
///// An instance of system'LiteralValue class. 
//#subject literal    = system'LiteralValue.
//
//#subject wide       = system'WideLiteralValue.
//
//// === function types ===
//#subject func       = system'BaseFunction.
//
///// An anonymous function with a generic parameter.
/////
///// An instance of system'BaseFunction1
//#subject func1      = system'BaseFunction1.
//
//#subject func2      = system'BaseFunction2.
//#subject func3      = system'BaseFunction3.
//#subject func4      = system'BaseFunction4.
//#subject func5      = system'BaseFunction5.
//#subject func6      = system'BaseFunction6.
//#subject func7      = system'BaseFunction7.
//#subject func8      = system'BaseFunction8.
//#subject func9      = system'BaseFunction9.
//#subject func10     = system'BaseFunction10.
//#subject func11     = system'BaseFunction11.
//
//#subject nfunc      = system'IndexFunction1.
//#subject nnfunc     = system'IndexFunction2.
//#subject n3func     = system'IndexFunction3.
//
//// === system handles ===
//#subject handle     = system'Handle.
//#subject rt_ptr     = system'RTPointer.
//#subject dirty_ptr  = system'UnsafePointer.
//
//// --- Enumerators / Indexers ---
//#subject enumerator = system'Enumerator.
//#subject iterator   = system'Iterator.
