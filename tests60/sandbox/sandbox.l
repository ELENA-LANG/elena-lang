import extensions;

singleton Helper
{
   yield Enumerator func(/*int start, int number*/)
   {
      $yield 0;
      $yield 2;
      $yield 1;

//      for (int i = 0; i < number; i++)
//      {
          //$yield start + 2 * i;
//      }
   }
}

//Enumerator func(int start, int number)
//{
//   for (int i = 0; i < number; i++)
//   {
//       $yield start + 2 * i;
//   }
//}

public program()
{
   //var enum := func(2, 10);
   var enum := Helper.func(/*2, 10*/);
   foreach(var item; in /*func(2, 10)*/enum)
   {
      console.writeLine(item)
   }
}


//singleton InfinitLoop
//{
//   static Enumerator _enum;
//
//   private Enumerable loop()
//   {
//      int i := 0;
//      while (true) {
//         $yield i;
//
//         i++;
//      }
//   }
//
//   int nextNumber()
//   {
//      var enum := _enum ?? _enum := loop().getEnumerator();
//
//      enum.next();
//
//      ^ *enum;
//   }
//}
