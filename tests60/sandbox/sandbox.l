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

singleton A
{
  string Val
     = "Hello";
}


public program()
{
   __intermediate s := /*"Hello"*/A.Val;

   Console.writeLine(s)
}
