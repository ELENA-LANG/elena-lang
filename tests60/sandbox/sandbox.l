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

namespace IndexedTest
{
   A
   {
      indexed foo()
         = "from A";
   }
   
   B : A
   {
      indexed foo()
         = "from B";   
   }   
}

public program()
{
   IndexedTest'A a := new IndexedTest'A();
   Assert.ifEqual(a.foo(), "from A");

   var o := a;
   Assert.ifEqual(o.foo(), "from A");
}
