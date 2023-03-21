import extensions;

singleton Si
{
   static object field;

   set()
   {
      field := 2;
   }

   print()
   {
      console.writeLine(field)
   }
}


public program()
{
   Si.set();

   Si.print();

//   var handle := Thread.assign(
//   {
//      console.writeLine("Hello from another thread");
//   });
//        
//   handle.start()
//
//   console.readKey();
}