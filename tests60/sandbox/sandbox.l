import system;

//singleton Value
//{
//   if(action)
//   {
//      action()
//   }
//}

A
{
   int Length
   {
      get()
        = 2;
   }
}

singleton Tester
{
   run()
   {
      //Output.write("Hello");

//      var a := Value;
//      a.if({ Output.write() });
   }
}

public singleton WinOutput
{
   method write(s)
   {
       int h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);
       extern KERNEL32.WriteConsoleA(h,s,5,0,0);
   }
}

public singleton LinuxOutput
{
   method write(s)
   {
       extern libc.write(1, s, 5);
   }
}

Output = WinOutput;
//Output = LinuxOutput;

public symbol program = Tester.run();
