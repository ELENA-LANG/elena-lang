import system;

singleton Tester
{
   run()
   {
      var a := true;

      if(a) 
      {
         Output.write("Hello From ELENA 6.0"$10);
      };
   }
}

public singleton WinOutput
{
   method write(s)
   {
       int h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);
       int l := s.Length;
       extern KERNEL32.WriteConsoleA(h,s,l,0,0);
   }
}

public singleton LinuxOutput
{
   method write(s)
   {
       int l := s.Length;
       extern libc.write(1, s, l);
   }
}

Output = WinOutput;
//Output = LinuxOutput;

public symbol program = Tester.run();
