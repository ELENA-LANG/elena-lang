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
   Console.printLine("sample2.1");

   var event := CountDownEvent.new(2);

   Console.printLine("sample2.2");

   ThreadPool.queueAction(&WorkItem3, event);
   ThreadPool.queueAction(&WorkItem4, event);
   
   Console.printLine("sample2.3");

   event.wait();

   Console.writeLine("Main thread exits")
}