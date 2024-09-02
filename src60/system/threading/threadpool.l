//import system'collections'threadsafe;
//import system'runtime;
//
//public sealed class ThreadPool
//{
//   private constructor() {}
//
//   static BlockingQueue<Func> _workItems := new BlockingQueue<Func>();
//
//   static constructor()
//   {
//      int cpuCount := Environment.ProcessorCount;
//      for(int i := 0; i < cpuCount; i++) {
//         Thread.assign(
//         {
//            while (true) {
//               Func f := _workItems.pop();
//
//               f();
//            }
//         });
//      }
//   }
//}