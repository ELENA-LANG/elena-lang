import extensions;

public sealed const struct Handle
{
   embeddable __ptr value;

   constructor(pointer ptr)
   {
      value := ptr
   }

   bool isUnassigned()
   {
      ^ value == 0;
   }
}


public program()
{

//   var handle := Thread.assign(
//   {
//      console.writeLine("Hello from another thread");
//   });
//        
//   handle.start()
//
//   console.readKey();
}