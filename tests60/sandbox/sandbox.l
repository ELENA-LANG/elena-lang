import extensions;
import system'threading;

public program()
{
   var handle := Thread.assign(
   {
      console.writeLine("Hello from another thread");
   });
        
   handle.start();

   console.readChar();
}