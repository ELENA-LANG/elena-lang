import system;

singleton B
{
    testB(a) 
    {
       a.testA();
    }
}

class A
{
   testA() 
   {
      var a := 2;
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

