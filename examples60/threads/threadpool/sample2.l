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

public sample2()
{
   var event := CountDownEvent.new(2);

   ThreadPool.queueAction(&WorkItem3, event);
   ThreadPool.queueAction(&WorkItem4, event);
   
   event.wait();

   Console.writeLine("Main thread exits")
}