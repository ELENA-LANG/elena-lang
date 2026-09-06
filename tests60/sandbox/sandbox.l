//import system'calendar;
//import system'threading;
//
//public class ThreadLocal
//{
//   __threadvar Date _current;
//   
//   constructor()
//   {
//      _current := Date.Now;
//   }
//   
//   printCurrent()
//   {
//      Console.writeLine(_current.toPrintable());
//   }   
//}
//
//public Program()
//{
//   auto t1 := Thread.start(
//   {
//      Thread.sleep(1000);
//      
//      var l := new ThreadLocal();
//      
//      Thread.sleep(10000);      
//      
//      l.printCurrent();      
//   });
//   
//   auto t2 := Thread.start(
//   {
//      Thread.sleep(5000);
//      
//      var l := new ThreadLocal();
//      
//      Thread.sleep(10000);
//      
//      l.printCurrent();      
//   });
//
//   system'runtime'GCManager.collectMinor();
//   
//   t1.join();
//   t2.join();
//}

//A;

import extensions;

/*public sealed XReference<T>
{
   embeddable T value;

   constructor(T val)
   {
      value := val
   }

   get T Value() : info("Returns the variable value")
      = value;

   set Value(T val)
      : info("Set the variable value")
   {
      value := val
   }
}*/

//public B2
//{
//   XReference<string> ref;
//   
//   constructor()
//   {
//      ref := "abc";
//   }
//   
//   set(string s)
//   {
//      //ref.Value := n
//      *ref := s
//   }
//
//   test()
//   {
//      Console.printLine("ref=", *ref);
//   }
//}

/*public class A;

public sealed X
{
   embeddable A value;

   constructor(A val)
   {
      value := val
   }
}*/


// --- byRefTests ---

singleton ByRefProber
{
/*   set(ref int retVal)
   {
      retVal := 123;
   }
*/
   set(ref string retVal)
   {
      retVal := "hello";
   }
}

public byRefTests()
{
   string s := "none";
   ByRefProber.set(ref s);

   Assert.ifTrue(s == "hello");
   Console.write(".");

   string s2 := "none";
   o.set(ref s2);

   Assert.ifTrue(s2 == "hello");
   Console.write(".");
}

public Program()
{
   byRefTests();

//   A a/* := new A()*/;
//   X x := a;

//   XReference<string> ref := "abc";
   
//   B2 b := new B2();
//   b.set("abc").test()

//   Array<A?> list := class Array<A?>.allocate(3);
}
