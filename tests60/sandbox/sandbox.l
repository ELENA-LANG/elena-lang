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
      indexed foo(string s)
         = "from A with " + s;

      indexed foo(int n)
         = "from A with int";
   }
   
   B : A
   {
      indexed foo(string s)
         = "from B with "+s;   
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

   Assert.ifEqual(a.foo("Hello"), "from A with Hello");
   Assert.ifEqual(weak a.foo("Hello2"), "from A with Hello2");
   console.write(".");

   IndexedTest'A b := new IndexedTest'B();
   Assert.ifEqual(b.foo("Hello"), "from B with Hello");
   Assert.ifEqual(weak b.foo("Hello"), "from B with Hello");

   IndexedTest'A c := new IndexedTest'C();
   Assert.ifEqual(c.foo("Hello2"), "from A with Hello2");
   Assert.ifEqual(weak c.foo("Hello2"), "from A with Hello2");
   console.write(".");
}
