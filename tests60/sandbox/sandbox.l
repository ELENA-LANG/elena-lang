import system;

singleton ByRefProber
{
//   set(ref int retVal)
//   {
//      retVal := 123;
//   }

   set(ref string retVal)
   {
      retVal := "hello";
   }
}

public program() 
{
   string s := "none";
   ByRefProber.set(ref s);

   console.writeLine(s == "hello");
}
