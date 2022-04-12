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
       int h := extern KERNEL32.GetStdHandle();

       //int h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);
//       extern KERNEL32.WriteConsoleW(h,"hello",4,0,0);
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

