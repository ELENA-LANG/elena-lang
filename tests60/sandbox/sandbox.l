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
   {
      Console.writeLine("Fired!") 
   }
}

public program()
{
   A? a := nil;

   Console.write("a:");
   a?.testMe();
   
   A? a2 := new A();

   Console.write("a2:");
   a2?.testMe();   
}
