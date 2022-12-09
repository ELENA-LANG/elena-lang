import extensions;

// --- intTests ---

intTests()
{
    console.write("intTests");

    int n := 2;
    int m := 3;
    int k := 4;
                    
    Assert.ifTrue(2 + 3 * 4 == 14);
    Assert.ifTrue(n + m * k == 14);
    console.write(".");
    Assert.ifFalse(n == m);
    Assert.ifTrue(n == n);
    Assert.ifTrue(n < m);
    Assert.ifFalse(n > m);
    Assert.ifTrue(n <= m);
    Assert.ifTrue(n <= n);
    Assert.ifTrue(n >= n);
    Assert.ifFalse(n >= m);
    console.write(".");

    int i := 1;
    i += 2;
    i -= 2;
    i *= 2;
    i /= 2;    
    Assert.ifTrue(i == 1);
    console.writeLine(".");
}

// --- longTests ---

longTests()
{
    console.write("longTests");

    int n32 := 23456;
    long n64 := n32;
    Assert.ifTrue(n64 == 23456l);
    console.write(".");

    long n := 2l;
    long m := 3l;
    long k := 4l;
                 
    Assert.ifTrue(2l + 3l * 4l == 14l);
    Assert.ifTrue(n + m * k == 14l);
    console.write(".");

    n := 2000000l;
    m := 500000l;    
    Assert.ifTrue(n * m == 1000000000000l);
    console.write(".");

    n := 12345678l;
    m := 12345679l;
    Assert.ifFalse(n == m);
    Assert.ifTrue(n == n);
    Assert.ifTrue(n < m);
    Assert.ifFalse(n > m);
    Assert.ifTrue(n <= m);
    Assert.ifTrue(n <= n);
    Assert.ifTrue(n >= n);
    Assert.ifFalse(n >= m);
    console.writeLine(".");
}

// --- realTests ---

realTests()
{
    console.write("realTests");

    real r := 2.89787;
    Assert.ifFalse(r == 2.8);
    Assert.ifTrue(r == 2.89787);
    console.write(".");

    real n := 2.0;
    real m := 3.0;
    real k := 4.0;

    Assert.ifTrue(n + m * k == 14.0);
    console.write(".");

    real i := 1.0;
    i += 2.1;
    i -= 2.1;
    i *= 2.0;
    i /= 2.0;
    Assert.ifTrue(i == 1.0);
    console.write(".");
    
    Assert.ifFalse(n == m);            
    Assert.ifTrue(n != m);            
    Assert.ifTrue(n < k);            
    Assert.ifFalse(n > m);            
    Assert.ifFalse(n >= k);            
    Assert.ifFalse(n >= m);            
    Assert.ifTrue(n >= n);            
    Assert.ifTrue(n <= n);            
    Assert.ifTrue(n < m);            
    Assert.ifTrue(m > n);            
    console.writeLine(".");
}

// --- gcTests ---

gcTests()
{
   console.write("gcTests");
   for (int i := 0, i < 100000, i := i + 1)
   {
      new IntNumber();
   };
   console.writeLine(".");
}

// --- stringTests ---
stringTests()
{
    console.write("stringTests");
    string s := "abc";

    Assert.ifTrue(s == "abc");
    console.write(".");
    string s2 := "de";
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde");
    console.writeLine(".");
}

// --- shortTests ---

shortTests()
{
    console.write("shortTests");
    short w := 1234;
    Assert.ifTrue(w == 1234);
    console.writeLine(".");
}

// --- conversionTests ---

conversionTests()
{
    console.write("conversionTests");
    int n := 1234;
    short w := n;
    Assert.ifTrue(w == 1234);
    console.write(".");
    string s := n.toString();
    Assert.ifTrue(s == "1234");
    console.write(".");
    int n2 := s.toInt();
    Assert.ifTrue(n2 == n);
    console.writeLine(".");
}

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
