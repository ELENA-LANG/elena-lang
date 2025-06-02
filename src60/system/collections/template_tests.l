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
      List<T1>? o1 := nil;
      Tuple<T1>? t1 := nil;
      Tuple<T1,T2>? t2 := nil;
      VarTuple<T1,T2>? vt2 := nil;
      Tuple<T1,T2,T3>? t3 := nil;
      Tuple<T1,T2,T3,T4>? t4 := nil;
      Tuple<T1,T2,T3,T4,T5>? t5 := nil;
      Tuple<T1,T3,T4,T5,T6>? t6 := nil;
      Map<T1,T2>? o2 := nil;
      Queue<T1>? o3 := nil;
      Stack<T1>? o4 := nil;
      SortedList<T1,T2>? sl := nil;
   }
}
