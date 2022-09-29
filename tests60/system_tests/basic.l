import system;
import extensions;

// --- assignTests ---

AssignTestClass
{
   field x;
   field y;

   constructor(x, y)
   {
      this x := x;
      this y := y;
   }

   test(x, y)
   {
      Assert.ifTrue(this x == x);
      Assert.ifTrue(this y == y);
   }
}

assignTests()
{
   console.write("assignTests");
   var o := new AssignTestClass(2,"test");
   o.test(2,"test");

   console.writeLine(".");
}

// --- boxingTests ---

struct BoxingStruct
{
   int x;
   int y;

   constructor()
   {
      x := 1;
      y := 2;
   }

   set(int x, int y)
   {
      this x := x;
      this y := y;
   }

   test(int x, int y)
   {
      Assert.ifTrue(this x == x);
      Assert.ifTrue(this y == y);
   }
}

singleton BoxingProber
{
   check(int n)
   {
      Assert.ifTrue(n == 123);
   }

   set(BoxingStruct bs)
   {
      bs.set(3,4)
   }
}

boxingTests()
{
   console.write("boxingTests");

   int n := 123;

   // --- boxing read-only struct
   // 1a. passing directly
   BoxingProber.check(n);
   console.write(".");

   // 1b. weak passing directly
   var o := BoxingProber;
   o.check(n);
   console.write(".");

   // --- boxing / unboxing variable struct
   BoxingStruct st;
   BoxingProber.set(st);
   st.test(3,4);
   console.write(".");

   BoxingStruct st2;
   var p := BoxingProber;
   p.set(st2);
   st2.test(3,4);
   console.writeLine(".");
}

// --- byRefTests ---

singleton ByRefProber
{
   set(ref int retVal)
   {
      retVal := 123;
   }

   set(ref string retVal)
   {
      retVal := "hello";
   }
}

byRefTests()
{
   console.write("byRefTests");

   int n := 0;
   ByRefProber.set(ref n);

   Assert.ifTrue(n == 123);
   console.write(".");

   int m := 0;
   var o := ByRefProber;
   o.set(ref m);

   Assert.ifTrue(m == 123);
   console.write(".");

   string s := "none";
   ByRefProber.set(ref s);

   Assert.ifTrue(s == "hello");
   console.write(".");

   string s2 := "none";
   o.set(ref s2);

   Assert.ifTrue(s2 == "hello");
   console.writeLine(".");
}

// --- gcTests ---

//gcTests()
//{
//   console.write("gcTests");
//   for (int i := 0, i < 10000, i := i + 1)
//   {
//      new IntNumber();
//   };
//   console.writeLine(".");
//}
