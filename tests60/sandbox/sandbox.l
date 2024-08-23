import extensions;
import system'threading;

myFunction()
{
   console.writeLine("Hello from the thread");
}

public program()
{
   auto myThread := Thread.assign(&myFunction);
   myThread.start();

   console.readChar()
}
