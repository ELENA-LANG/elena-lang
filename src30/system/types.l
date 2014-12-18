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
#type(class:system'Integer)      intvar.
#type(class:system'Long)         longvar.
#type(class:system'Real)         realvar.
#type(class:system'CharValue)    char.

// === int synonyms ===
#type(type:int) length.
#type(type:int) index.

// === other synonyms ===
#type(type:literal)path.

// === other types ===
#type bool.

// === nested functions ===

#type(class:system'IndexFunction)indexfunc.
#type(class:system'IndexFunction2)matrixfunc.
#type(class:system'IndexFunction3)coordfunc.

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

#type(type:func1)forEach.
#type(type:func1)seeking.
#type(type:indexfunc)every.
#type(type:indexfunc)forEvery.

// --- Enumerator ---

#type(class:system'Enumerator)enumerator.
#type(class:system'Iterator)iterator.

// === extension types ===
#type(unforced)control.
#type(unforced)textio.
#type(unforced)enumerable.
#type(unforced)indexable.
