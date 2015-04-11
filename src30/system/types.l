// ==== ver 3.1.1 ===

// === basic stack allocated types ===

#subject(class:system'ByteNumber)   byte.
#subject(class:system'ShortNumber)  short.
#subject(class:system'IntNumber)    int.
#subject(class:system'LongNumber)   long.
#subject(class:system'RealNumber)   real.

#subject(class:system'Signature)    subject.
#subject(class:system'Message)      mssg.
#subject(class:system'Message)      verb.

#subject(class:system'ByteArray)    bytearray.
#subject(class:system'ShortArray)   shortarray.
#subject(class:system'IntArray)     intarray.
#subject(class:system'CharArray)    chararray.
#subject(class:system'Array)        array.

// === basic dynamic types ===
#subject(class:system'LiteralValue)     literal.
#subject(class:system'WideLiteralValue) wide.

#subject(class:system'OpenArgs)         params.
#subject(class:system'OpenArgs)         args.
#subject(class:system'CharValue)        char.

#subject(class:system'Integer)          vint.
#subject(class:system'Long)             vlong.
#subject(class:system'Real)             vreal.
#subject(class:system'Byte)             vbyte.
#subject(class:system'Short)            vshort.
#subject(class:system'Char)             vchar.

// === int synonyms ===
#subject(class:system'IntNumber)        length.
#subject(class:system'IntNumber)        index.
#subject(class:system'IntNumber)        offset.
#subject(class:system'IntNumber)        int_attributes.

#subject(class:system'IntNumber)        x.
#subject(class:system'IntNumber)        y.
#subject(class:system'IntNumber)        width.
#subject(class:system'IntNumber)        height.

// === vint synonyms ===
#subject(class:system'Integer)          vlength.

// === literal synonyms ===
#subject name.
#subject path.

// === subject synonyms ===
#subject(class:system'Signature)    error_type.

// === logical types ===
#subject(class:system'BaseBoolValue) bool.

#subject string.

// === nested functions ===

#subject(class:system'IndexFunction1)nfunc.
#subject(class:system'IndexFunction2)nnfunc.
#subject(class:system'IndexFunction3)n3func.

// === function types ===
#subject(class:system'BaseFunction)  func.
#subject(class:system'BaseFunction1) func1.
#subject(class:system'BaseFunction2) func2.
#subject(class:system'BaseFunction3) func3.
#subject(class:system'BaseFunction4) func4.
#subject(class:system'BaseFunction5) func5.
#subject(class:system'BaseFunction6) func6.
#subject(class:system'BaseFunction7) func7.
#subject(class:system'BaseFunction8) func8.
#subject(class:system'BaseFunction9) func9.
#subject(class:system'BaseFunction10)func10.
#subject(class:system'BaseFunction11)func11.
#subject(class:system'BaseFunction12)func12.
#subject(class:system'BaseFunction13)func13.
#subject(class:system'BaseFunction14)func14.

#subject(class:system'BaseFunction1) each.
#subject(class:system'IndexFunction1)every.
//#subject(type:indexfunc)forEvery.

// --- Enumerators / Indexers ---

#subject(class:system'Enumerator)enumerator.
#subject(class:system'Iterator)iterator.

// --- basic properties ---
#subject key.

// === extension types ===
#subject enumerable.
#subject indexable.

// === exception types ===
#subject(class:system'BaseFunction1) Error.
#subject(class:system'BaseFunction1) MethodNotFoundError.
#subject(class:system'BaseFunction1) FormatError.
#subject(class:system'BaseFunction1) OutOfRangeError.
#subject(class:system'BaseFunction1) InvalidArgumentError.
#subject(class:system'BaseFunction1) NotSupportedError.
#subject(class:system'BaseFunction1) Abort.

// === system handles ===
#subject(class:Handle) handle.
#subject(class:system'RTPointer)rt_ptr.
#subject(class:system'UnsafePointer)dirty_ptr.
