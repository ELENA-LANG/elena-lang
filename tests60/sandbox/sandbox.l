import extensions;

// --- methodNotFoundTest ---

nilReferenceTest()
{
   console.write("nilReferenceTest:");

   var o := nil;
   var f := false;
   try
   {
      o.fail()
   }
   catch(NilReferenceException e)
   {
      f := true;
   };

   Assert.ifTrue(f);

   console.writeLine(".");
}
 
public program()
{
   nilReferenceTest();
}