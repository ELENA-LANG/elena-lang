namespace collections
{
   T1;
   T2;
   T3;
   T4;
   T5;
   T6;
   TKey;

   collections_template_test()
   {
      var List<T1> o1 := nil;
      var Tuple<T1> t1 := nil;
      var Tuple<T1,T2> t2 := nil;
      var VarTuple<T1,T2> vt2 := nil;
      var Tuple<T1,T2,T3> t3 := nil;
      var Tuple<T1,T2,T3,T4> t4 := nil;
      var Tuple<T1,T2,T3,T4,T5> t5 := nil;
      var Tuple<T1,T3,T4,T5,T6> t6 := nil;
      var Map<T1,T2> o2 := nil;
      var Queue<T1> o3 := nil;
      var Stack<T1> o4 := nil;
      var SortedList<T1,T2> sl := nil;
   }
}
