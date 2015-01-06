// === basic stack allocated types ===

#type(class:system'ByteNumber)  byte.
#type(class:system'ShortNumber) short.
#type(class:system'IntNumber)   int.
#type(class:system'LongNumber)  long.
#type(class:system'RealNumber)  real.

//#type(size:4)intptr.
#type(class:system'Signature)subject.

#type(class:system'ByteArray)  bytearray.
#type(class:system'ShortArray) shortarray.
#type(class:system'Array)      array.

// === basic dynamic types ===
#type(class:system'LiteralValue) literal.
#type(class:system'OpenArgs)     params.
#type(class:system'OpenArgs)     args.
#type(class:system'Integer)      intvar.
#type(class:system'Long)         longvar.
#type(class:system'Real)         realvar.
#type(class:system'CharValue)    char.

// === int synonyms ===
#type(class:system'IntNumber) length.
#type(class:system'IntNumber) index.
#type(class:system'IntNumber) int_attributes.

// === literal synonyms ===
#type(class:system'LiteralValue)name.
#type(class:system'LiteralValue)path.

// === subject synonyms ===
#type(class:system'Signature) error_type.

// === logical types ===
#type(class:system'BaseBoolValue) bool.

// === nested functions ===

#type(class:system'IndexFunction1)nfunc.
#type(class:system'IndexFunction2)nnfunc.
#type(class:system'IndexFunction3)n3func.

// === function types ===
#type(class:system'BaseFunction)  func.
#type(class:system'BaseFunction1) func1.
#type(class:system'BaseFunction2) func2.
#type(class:system'BaseFunction3) func3.
#type(class:system'BaseFunction4) func4.
#type(class:system'BaseFunction5) func5.
#type(class:system'BaseFunction6) func6.
#type(class:system'BaseFunction7) func7.
#type(class:system'BaseFunction8) func8.
#type(class:system'BaseFunction9) func9.
#type(class:system'BaseFunction10)func10.
#type(class:system'BaseFunction11)func11.
#type(class:system'BaseFunction12)func12.
#type(class:system'BaseFunction13)func13.
#type(class:system'BaseFunction14)func14.

#type(class:system'BaseFunction1) each.
#type(class:system'IndexFunction1)every.
//#type(type:indexfunc)forEvery.

// --- Enumerators / Indexers ---

#type(class:system'Enumerator)enumerator.
#type(class:system'Iterator)iterator.

// === extension types ===
#type textio.
#type enumerable.
#type indexable.

// === exception types ===
#type(class:system'BaseFunction1) Error.
#type(class:system'BaseFunction1) MethodNotFoundError.
#type(class:system'BaseFunction1) FormatError.
#type(class:system'BaseFunction1) OutOfRangeError.
#type(class:system'BaseFunction1) InvalidArgumentError.
#type(class:system'BaseFunction1) NotSupportedError.
#type(class:system'BaseFunction1) Abort.

// === system handles ===
#type(class:Handle) handle.
