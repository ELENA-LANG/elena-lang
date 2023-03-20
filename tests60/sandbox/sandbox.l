import extensions;

MyClass
{
   Reference<int> R : prop;

   constructor()
   {
      R := 0;
   }
}

public program()
{
   var r := new MyClass();
   r.R.Value := 2;

   console.writeLine(*r.R);


//   var handle := Thread.assign(
//   {
//      console.writeLine("Hello from another thread");
//   });
//        
//   handle.start()
//
//   console.readKey();
}