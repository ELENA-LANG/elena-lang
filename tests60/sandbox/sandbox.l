import system;

interface X
{
    abstract x();
}

XX : X
{
   x() {} 
}

public singleton WinOutput
{
   method output()
   {
       int h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);
       extern KERNEL32.WriteConsoleA(h,"hello",5,0,0);
   }
}

public singleton LinuxOutput
{
   method output()
   {
       extern libc.write(1, "hellow", 5);
   }
}

Output = WinOutput;

singleton B
{
    testB(a) 
    {
       a.testA();
    }
}

class A
{
   private test() 
   {
   }

   testA() 
   {
      self.test();

      Output.output();
   }

   constructor new() {}
}
                                

public symbol program = B.testB(A.new());

