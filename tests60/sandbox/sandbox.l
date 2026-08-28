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

public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public Program()
{
   Color color := Color.Red;
   
   if (color == Color.Red) {
      Console.writeLine("Do work!")
   }
   else Console.writeLine("Does not work");
}
