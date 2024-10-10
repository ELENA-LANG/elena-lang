import extensions;
//import system'collections;
//
//public extension testOp<T>
//{
//   Array</*T*/int> toXArray()
//   {
//      List<T> list := new List<T>();
//      foreach(T item; in self)
//      {
//         list.append(item)
//      };
//
//      ^ list.Value
//   }
//}
//
//
//public program()
//{
////   var array := Range.for(0, 10).toXArray();
//   var r := Range.for(0, 10);
//   var array := r.toXArray<int>();
//}

A<T>
{
   static giveMe()
      = new A<T>();
}

public program()
{
   A<int> o3 := new A<int>();

   var o := new A<int>();

   var o2 := ::A<int>.giveMe();
   var o4 := class A<int>.giveMe();


//   var l := new List<int>();
}