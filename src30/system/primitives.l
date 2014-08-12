
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
//
