import extensions;
import system'threading;

//myFunction()
//{
//   console.writeLine("Hello from the thread");
//}
//
//public program()
//{
//   auto myThread := Thread.assign(&myFunction);
//
//   var priority := myThread.Priority;
//
//   console.printLine("Thread priority ", priority);
//
//   myThread.start();
//
//   console.readChar()
//}

myFunction(arg)
{
   console.writeLine($"{arg} from the thread");

   console.readChar()
}

public program()
{
   auto myThread := Thread.assign(&myFunction);

   var priority := myThread.Priority;

   console.printLine("Thread priority ", priority);

   myThread.start("Good bye");

   myThread.join(2000);
}
