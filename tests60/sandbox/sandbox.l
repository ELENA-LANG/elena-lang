import extensions;

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

A
{
   testMe()
      = "Fired!";
}

public program()
{
   A a := new A();

   var r1 := a\.testMe();
   Console.writeLine(r1);
   var r2 := a\.testMe2();
   Console.writeLine(r2 ?? "<nil>");
}
