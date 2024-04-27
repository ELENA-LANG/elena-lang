//import extensions;

B;

myMethod(B arg)
{
}

singleton TestHelper
{
   myMethodInvoker()
   {
      var b1 := new B();
      var b2 := new B();
      myMethod(b1, b2)
   }

   myMethod(params B[] arg) 
   {
   }
}

public program()
{
   TestHelper.myMethodInvoker()
}