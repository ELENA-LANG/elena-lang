// === basic stack allocated types ===

#type(size:4,wrapper:system'IntNumber)  int.
#type(size:8,wrapper:system'LongNumber) long.
#type(size:8,wrapper:system'RealNumber) real.
#type(size:2,wrapper:system'ShortNumber)short.

#type(itemsize:1,wrapper:system'ByteArray) bytearray.
#type(itemsize:2,wrapper:system'ShortArray)shortarray.

// === basic dynamic types ===
#type(wrapper:system'LiteralValue)literal.
#type params.

// === int synonyms ===
#type(synonym:int) length.
#type(synonym:int) index.

// === basic type extensions ===

// --- referenceTableControl ---

#symbol referenceTableControl =
{
    allocate : anObject &out'int:anIndex
        <api_alloc_index>.

    free &int:anIndex
        <api_free_index>.
}.

//// --- mathPrimitive ---
//
//#symbol mathPrimitive =
//{
//    Exp &real:aValue &out'real:aRetVal
//        <r_exp>.
//        
//    Ln &real:aValue &out'real:aRetVal
//        <r_ln>.
//        
//    Abs &real:aValue &out'real:aRetVal
//        <r_abs>.
//        
//    Round &real:aValue &out'real:aRetVal
//        <r_round>.
//        
//    Truncate &real:aValue &out'real:aRetVal
//        <r_truncate>.
//}.

