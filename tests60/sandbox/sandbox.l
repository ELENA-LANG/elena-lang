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

__threadvar ThreadVar
   = new A();

public program()
{
   var a := ThreadVar;
   var b := ThreadVar;

   extensions'threading'threadControl.start({ var b := ThreadVar; });

   console.readChar()
}
