import extensions;

public program()
{
   /*int*/var ret := 0;
   var o := new object();
   try
   {
      o.fail()
   }
   catch(Exception ex)
   {
      ret := -1
   };
   console.writeLine(ret);

}
