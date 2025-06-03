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

/*A<T>
{
   static A<T>? Default
      = nil;      
}*/

B;

A
{
   B Item : prop;
}

public program()
{
    A? a := nil;

    var r := (a ?? a := new A()).Item;
	
//   A<int> n := new A<int>();
   //List<int>? list := nil;
}
