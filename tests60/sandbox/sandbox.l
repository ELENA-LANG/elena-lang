import system;

singleton Tester
{
   run()
   {
       for (int n := 0, n < 5, n := n + 1) {
          Output.write("*");
       }

//     int n := 0;
//       $rep (n < 5) ? { Output.write("*"); n := n + 1; };

//       int n := 0;
//       while (n < 5) {
//          Output.write("*"); n := n + 1; 
//       }

//      var r1 := 2;
//      var r2 := 3;

//      if(r1 == r1) 
//      {
//          Output.write("equalReference works"$10);
//      };

//      if(r1 == r2) 
//      {
//          Output.write("equalReference doesn't works"$10);
//      }

//      var a := true;

//      if(a) 
//      {
//         Output.write("Hello From ELENA 6.0"$10);
//      };
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
