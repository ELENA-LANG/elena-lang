import system'threading;
import extensions;
import extensions'threading;

WorkItem1(state)
{
   Console.printLineConcurrent("WorkItem1: ", state);
}

WorkItem2(state)
{
   int num := state.toInt();
   int square := num * num;
   Console.printLineConcurrent($"WorkItem2: {num} * {num} = {square}");
}

public sample1()
{
   ThreadPool.queueAction(&WorkItem1, "Hello");
   ThreadPool.queueAction(&WorkItem2, 42);
   
   Thread.sleep(1000);

   Console.printLineConcurrent("Main thread exits")
}