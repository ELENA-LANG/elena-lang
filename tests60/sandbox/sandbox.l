import extensions;

public program()
{
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
}
