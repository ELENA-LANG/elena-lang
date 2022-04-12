import system;

public C;
C;

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
       ^ self;
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

