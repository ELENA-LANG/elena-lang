import extensions;

singleton Helper
{
//   int, bool getPair()
//   {
//      ^ (2,true)
//   }

   ::(int, bool) getPair2()
   {
      ^ (2,true)
   }
}

public program()
{
//   int n, bool b := Helper.getPair();
//   console.printLine(n,",",b)

   (int n2, bool b2) := Helper.getPair2();
   console.printLine(n2,",",b2)
}