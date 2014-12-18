//#define system.
//#define system'routines.
//#define system'dynamic.
//
//// --- controlEx ---
//
//#symbol controlEx =
//{
//    foreach:anEnumerable &top:aMaxValue &do:aFunction
//        = aFunction~functionOp run &enumerator:(TopEnumerator new:anEnumerable &max:aMaxValue) | stop.
//
//    foreach:anEnumerable &top:aMaxValue &where:aCondition &do:aFunction
//        = aFunction~functionOp run &each:(TopEnumerator new:anEnumerable &max:aMaxValue) &where:aCondition | stop.
//
//    foreach:anEnumerable &where:aCondition &do:aFunction
//        = aFunction~functionOp run &each:(anEnumerable enumerator) &where:aCondition | stop.
////
////    dispatch : anObject1 : anObject2 &table:aTable &to:aTarget
////        = aTarget~(aTable~EDispatchTable eval:anObject) eval:anObject1:anObject2.
//}.
