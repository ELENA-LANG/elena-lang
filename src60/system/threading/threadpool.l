import system;
import system'collections'threadsafe;
import system'runtime;

public sealed class ThreadPool
{
   private constructor() {}

   static BlockingQueue<Func> _actions := new BlockingQueue<Func>();

   static constructor()
   {
      int cpuCount := Environment.ProcessorCount;
      for(int i := 0; i < cpuCount; i++) {
         Thread.new(
         {
            while (true) {
               Func f := _actions.pop();

               f();
            }
         });
      }
   }

   static queueAction(Func f)
   {
      _actions.push(f);
   }

   static queueAction(Func1 f, object arg)
   {
      _actions.push(new StartHelper(f, arg));
   }
}