import system;

public const struct String : BaseValue
{
    __string byte[] theArray;

    get int Length()
    {
       int l := $len theArray;
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
