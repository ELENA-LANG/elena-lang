/*import system'threading;
import extensions'threading;

async public program()
{
   Task t1 := Task.run({ Console.printLineConcurrent("Enjoy") });
   Task t2 := Task.run({ Console.printLineConcurrent("Rosetta") });
   Task t3 := Task.run({ Console.printLineConcurrent("Code") });

   :await Task.whenAllArgs(t1, t2, t3);
}
*/

import system'threading;
import extensions;
import extensions'threading;

WorkItem3(state)
{
   Console.printLineConcurrent("WorkItem1");

   state.signal()
}

WorkItem4(state)
{
   Console.printLineConcurrent("WorkItem2");

   state.signal()
}

public program()
{
   var event := CountDownEvent.new(2);

   ThreadPool.queueAction(&WorkItem3, event);
   ThreadPool.queueAction(&WorkItem4, event);
   
   event.wait();

   Console.writeLine("Main thread exits")
}