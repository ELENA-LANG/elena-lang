public sealed class A
{
   protected constructor()
   {
      console.writeLine("new()");
   }

   constructor assign()
   {
      console.writeLine("assign()");
   }
}

public program()
{
   var a := A.assign();
}
