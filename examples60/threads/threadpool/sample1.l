import system'threading;
import extensions'threading;

WorkItem1(state)
{
   console.writeLine($"WorkItem1: {state}");
}

WorkItem2(state)
{
   int num := state.toInt();
   int square := num * num;
   console.writeLine($"WorkItem2: {num} * {num} = {square}");
}

public sample1()
{
   ThreadPool.queueAction(&WorkItem1, "Hello");
   ThreadPool.queueAction(&WorkItem2, 42);
   
   //Thread.sleep(1000);
   console.readChar();

   console.writeLine("Main thread exits")
}