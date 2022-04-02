// Invoking a class symbol

//struct R
//{
//   /*embeddable */__int theValue[4];
//}

C;

D
{
   C c;
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
   b;

   testA() 
   {
      var a := self;
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

