// Invoking a class symbol

singleton B
{
    testB(a) 
    {
//       a.testA();
    }
}

class A
{
   testA() {}

   constructor new() {}
}
                                

//public symbol program = A.new().testA();
public symbol program = B.testB(A.new());

