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

closed class A
{
   foo()
   {
      console.writeLine("foo fired!")
   }
}

B : A
{
   indexed internal bar()
   {
      console.writeLine("bar fired!")
   }
}

public program()
{
   B b := new B();
   A a := b;

   b.bar();
   a.foo();
}
