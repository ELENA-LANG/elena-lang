import extensions;

class A
{
   constructor()
   {
      lock(console) {
         console.writeLine("Creating A");
      }
   }
}

singleton B
{
   __threadvar A singleton;

   getA()
   {
      if (singleton == nil)
         singleton := new A();

      ^ singleton
   }
}


public program()
{
   var a := B.getA();
   var b := B.getA();

   extensions'threading'threadControl.start({ var b := B.getA(); });

   console.readChar()
}
