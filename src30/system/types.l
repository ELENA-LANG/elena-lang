// === basic logical types ===
#subject bool       = system'BaseBoolValue.

// === basic primitive types ===

//#subject(class:system'KeyValue)     keyvalue.

#subject ref        = system'Variable.

#subject byte       = system'ByteNumber.
#subject short      = system'ShortNumber.
#subject int        = system'IntNumber.
#subject long       = system'LongNumber.
#subject real       = system'RealNumber.
#subject char       = system'CharValue.
#subject mssg       = system'Message.
//#subject(class:system'ExtensionMessage) ext_mssg.
#subject verb       = system'Verb.
#subject subject    = system'Signature.
//#subject(class:system'Symbol)       symbol_ref.

// === basic dynamic types ===
#subject bytearray  = system'ByteArray.
#subject shortarray = system'ShortArray.
#subject intarray   = system'IntArray.
//#subject(class:system'IntMatrix)    intmatrix.
//#subject(class:system'RealArray)    realarray.
//#subject(class:system'RealMatrix)   realmatrix.
//#subject(class:system'CharArray)    chararray.
#subject array      = system'Array.
//#subject(class:system'OpenArgs)     args.
//#subject(class:system'MessageArray) mssgarray.
//#subject(class:system'BitArray)     bitarray.
//#subject(class:system'BitArray32)   bitset.

#subject literal    = system'LiteralValue.
#subject wide       = system'WideLiteralValue.

// === function types ===
//#subject(class:system'BaseFunction)  func.
//#subject(class:system'BaseFunction1) func1.
//#subject(class:system'BaseFunction2) func2.
//#subject(class:system'BaseFunction3) func3.
//#subject(class:system'BaseFunction4) func4.
//#subject(class:system'BaseFunction5) func5.
//#subject(class:system'BaseFunction6) func6.
//#subject(class:system'BaseFunction7) func7.
//#subject(class:system'BaseFunction8) func8.
//#subject(class:system'BaseFunction9) func9.
//#subject(class:system'BaseFunction10)func10.
//#subject(class:system'BaseFunction11)func11.
//#subject(class:system'BaseFunction12)func12.
//#subject(class:system'BaseFunction13)func13.
//#subject(class:system'BaseFunction14)func14.
//
//#subject(class:system'IndexFunction1)nfunc.
//#subject(class:system'IndexFunction2)nnfunc.
//#subject(class:system'IndexFunction3)n3func.

// === system handles ===
#subject handle     = system'Handle.
//#subject(class:system'RTPointer)rt_ptr.
//#subject(class:system'UnsafePointer)dirty_ptr.

// --- Enumerators / Indexers ---
#subject enumerator = system'Enumerator.
//#subject(class:system'Iterator)iterator.
