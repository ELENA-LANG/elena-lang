//import extensions;

B;

myMethod(B arg)
{
}

singleton TestHelper
{
   myMethodInvoker()
   {
      myMethod(new B())
   }

   myMethod(B arg) 
   {
   }
}

public program()
{
   TestHelper.myMethodInvoker()
}