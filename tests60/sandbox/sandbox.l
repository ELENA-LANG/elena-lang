import extensions;

public program()
{
   console.write("methodNotFoundTest:");

   var o := 2;
   var f := false;
   try
   {
      o.fail()
   }
   catch(MethodNotFoundException e)
   {
      f := true;
      console.writeLine("error");
   };

   Assert.ifTrue(f);

   console.writeLine(".");
}