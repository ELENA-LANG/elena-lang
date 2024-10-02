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

A
{
   int getValue(int n)
      = 2 + n;
}

public program()
{
   A a := new A();

   int n := a.getValue(2);
}
