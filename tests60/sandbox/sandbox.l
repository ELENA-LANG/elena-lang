import extensions;
//import system'threading;
//import system'calendar;
//
//public program()
//{
//   auto t := class Task<int>.run({
//                               // Just loop.
//                               int max := 1000000;
//                               int ctr := 0;
//                               for (ctr := 0; ctr <= max; ctr++) {
//                                  if (ctr == max / 2 && Now.Hour <= 12) {
//                                     ctr++;
//                                     $break;
//                                  }
//                               };
//
//                               ^ ctr
//                            });
//   Console.printLine("Finished ", t.Result, " iterations.");  
//}

extension op
{
   int xmod(o)
   {
      ^ self.xmod(cast int(o))
   }

   int xmod(int n)
      = n + 3;
}

public program()
{
   int n := 2;

   int r := n.xmod(3);

//   int n := 10;
//   int radix := 3;
//
//   int r := n.mod(radix);
}
