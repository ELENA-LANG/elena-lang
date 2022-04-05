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
      var s := "abc";
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

