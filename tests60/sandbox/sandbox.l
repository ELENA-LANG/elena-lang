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

//import extensions;

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

//public Program()
//{
//   XReference<string> ref := "abc";
   
//   B2 b := new B2();
//   b.set("abc").test()

//   Array<A?> list := class Array<A?>.allocate(3);
//}

//import system'dynamic;
//import system'dynamic'expressions;
//import extensions;
//
//public interface I
//{
//   abstract string whoAmI();
//}
//
//singleton A
//{
//   whoAmI()
//      = "I'm a singleton A";
//}
//
//public Program()
//{ 
//   var tree := DynamicSingleton.load(
//                Expression.Method(
//                   "whoAmI",
//                   Expression.CodeBlock(
//                     Expression.Constant("I'm an expression tree") 
//                   )
//                )
//             );
//   
//   var mock := tree.compile();
//
//   I i1 := A.mockInterface(I);   
//   Console.printLine(i1.whoAmI());
//   
//   I i2 := mock.mockInterface(I);   
//   Console.printLine(i2.whoAmI());
//}

import extensions;
import extensions'scripting;

public Program()
{
    var text := Program_arguments[1];
    var arg := Program_arguments[2];
    
    var program := LScript.interpretLine(text);
    Console.printLine(
        text,",",arg," = ",program.eval(arg.toReal()));
}