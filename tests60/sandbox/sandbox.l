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

B
{
   private callMe()
   {
   }

   test()
   {
      self.callMe();
   }
}

A
{
   indexed internal foo(string s)
      = "from A with " + s;
}

public program()
{
   A a := new A();

   var r := a.foo("Hello");
   Console.writeLine(r)
}
