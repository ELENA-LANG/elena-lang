import extensions;

import system'collections;

//A
//{
//   int? x;

//   int? getX()
//      = x;

//   constructor()
//   {
//      x := nil;
//   }

//   test()
//   {
//      if:not:nil(x) {
//         /*var s :=*/ /*x*/self.getX()?.toPrintable();
//      }
//      x := 5;

//      int r := x; // !! no warning

//      var o := x; 
//   }
//}

class XList<T>
{
   T[]            _array;

   do()
   {
      T a := _array[0];
   }
}

public program()
{
   List<int?> list := nil;
}
