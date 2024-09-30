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

   C : A
   {
      indexed bar()
         = "from C";   
   }
}

public program()
{
   IndexedTest'A a := new IndexedTest'A();
   Assert.ifEqual(a.foo(), "from A");
   Assert.ifEqual(weak a.foo(), "from A");
   console.write(".");

   IndexedTest'A b := new IndexedTest'B();
   Assert.ifEqual(b.foo(), "from B");
   Assert.ifEqual(weak b.foo(), "from B");

   IndexedTest'A c := new IndexedTest'C();
   Assert.ifEqual(c.foo(), "from A");
   Assert.ifEqual(weak c.foo(), "from A");
   console.write(".");
}
