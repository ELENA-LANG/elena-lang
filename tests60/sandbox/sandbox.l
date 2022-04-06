import system;

C;

D
{
   C get()
   {
   }
}


singleton B
{
    testB(a) 
    {
       a.testA();
    }
}

C;

class A
{
   testA() 
   {
       ^ self;
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

