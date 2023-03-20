import extensions;

public program()
{
   var handle := Thread.assign(
   {
      console.writeLine("Hello from another thread");
   });
        
   handle.start()

   console.readKey();
}