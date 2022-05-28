import system;

//public extension commonOp
//{
//   bool equalReference(o) {}
//}

B;

sealed class A
{
   do(B b)
   {
      Output.write("Dispatch works!"$10);
   }

   do(A A)
   {
      Output.write("Dispatch doesn't works!"$10);
   }
}

singleton Tester
{
   run()
   {
      /*A*/var a := new A();
      var b := new B();

      a.do(b);

//      var r1 := 2;
//      var r2 := 3;

//      r1.equalReference(r1)
 
//      if(r1.equalReference(r1)) 
//      {
//          Output.write("equalReference works"$10);
//      };

//      if(r1.equalReference(r2)) 
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
