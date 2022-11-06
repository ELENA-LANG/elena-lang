A
{
   protected constructor() {}

   constructor new()
   {
   }
}

public program()
{
//   var a := new A();
   var a := A.new();
   console.writeLine(a);
//   console.writeLine(a.__getClass());
}
