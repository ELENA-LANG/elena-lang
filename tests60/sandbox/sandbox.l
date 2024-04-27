import extensions;

singleton TestHelper
{
   myMethodInvoker()
   {
      myMethod("TestHelper.myMethod fired")
   }

   myMethod(string s) 
   {
      console.writeLine(s);
   }
}

public program()
{
   TestHelper.myMethodInvoker()
}