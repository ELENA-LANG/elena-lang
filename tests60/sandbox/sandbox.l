C;

D : C
{
}

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
      var a := self;
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

