import system;

A;

B
{
   add(o)
   {
      console.writeLine("B.add(object)");
   }
   add(A a)
   {
      console.writeLine("B.add(A)");
   }
}


public program() 
{
   var a := new A();
   var o := new object();

   var b := new B();

   b.add(o);
   b.add(a);
}
