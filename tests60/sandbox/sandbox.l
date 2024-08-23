import extensions;
import system'threading;

myFunction()
{
   console.writeLine("Hello from the thread");
}

public program()
{
   auto myThread := Thread.assign(&myFunction);

   var priority := myThread.Priority;

   console.printLine("Thread priority ", priority);

   myThread.start();

   console.readChar()
}
