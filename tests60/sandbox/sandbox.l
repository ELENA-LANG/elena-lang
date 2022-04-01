// Invoking a class symbol

singleton B
{
    testB(a) 
    {
       a.testA();
    }
}

class A
{
   b;

   testA() 
   {
      var a := self;
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

