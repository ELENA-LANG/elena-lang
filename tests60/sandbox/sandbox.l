import system;

//inline statement<routine>()
//{
//   #let routine[@name] := @reference
//}

//if<expr>(truePart) //: statement<templates>
//{
//
//}

B;

class A<T>
{
   T field;

//   constructor new(T t)
//   {
//      this field := t;  
//   }
}

singleton Tester
{
   run()
   {
      A<B> a := nil;

//      var a := A<B>.new(new B());
//      var a := new A<B>();

//      Output.write("Hello From ELENA 6.0"$10);

//      bool a := false;

//      if(a) 
//      {
//         Output.write("Hello From ELENA 6.0"$10);
//      };

//      a ? { Output.write("Hello From ELENA 6.0"$10); }
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
