import extensions;

// --- intTests ---

intTests()
{
   console.write("intTests:");

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

   int m2 := 10;
   int k2 := -2;
   int l2 := -10;

   Assert.ifFalse(k2 > m2);
   Assert.ifTrue(m2 > k2);
   Assert.ifFalse(k2 < l2);
   console.write(".");
   
   int i := 1;
   i += 2;
   i -= 2;
   i *= 2;
   i /= 2;    
   Assert.ifTrue(i == 1);
   console.writeLine(".");
}

uintTests()
{
   console.write("uintTests:");

   uint k := 2H;
   uint l := 0AH;
   uint m := 081000000H;
   uint n := 08FFFFFFFH;

   Assert.ifFalse(k > l);  // 2 > 10
   Assert.ifFalse(m < l);  // 081000000H < 10
   Assert.ifTrue(l < m);   // 10 < 081000000H
   Assert.ifTrue(n > m);  // 08FFFFFFFH > 081000000H
   console.write(".");

   m := 80000000H;
   n := 10H;
   k := m / n;
   Assert.ifTrue(k == 8000000H);
   console.writeLine(".");
}


// --- longTests ---

longTests()
{
    console.write("longTests:");

    int n32 := 23456;
    long n64 := n32;
    int in32 := -23458;
    long in64 := in32;
    Assert.ifTrue(n64 == 23456l);
    Assert.ifTrue(in64 == -23458l);
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

    Assert.ifFalse(m == n);
    Assert.ifTrue(m > n);
    Assert.ifFalse(m < n);
    console.write(".");

    long l := 2l;
    long l2 := -2l;
    Assert.ifTrue(l == l);   
    Assert.ifTrue(l2 == l2); 
    Assert.ifFalse(l2 == l); 
    Assert.ifFalse(l == l2); 
    Assert.ifFalse(l < l2);  
    Assert.ifTrue(l > l2);   
    Assert.ifTrue(l2 < l);   
    Assert.ifFalse(l2 > l);  
    console.write(".");

    n := 2;
    int n2 := -2;

    Assert.ifTrue(l == n);   // ; true
    Assert.ifTrue(l2 == n2); // ; true
    Assert.ifFalse(l == n2);  // ; false
    Assert.ifFalse(l2 == n); // ; false

    Assert.ifFalse(l < n2);  // ; false
    Assert.ifFalse(l < n);  // ; false
    Assert.ifTrue(l2 < n);  // ; true
    Assert.ifFalse(l2 < n2);  // ; false

    console.writeLine(".");
}

// --- realTests ---

realTests()
{
    console.write("realTests:");

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
   console.write("gcTests:");
   for (int i := 0, i < 100000, i := i + 1)
   {
      new IntNumber();
   };
   console.writeLine(".");
}

// --- stringTests ---
stringTests()
{
    console.write("stringTests:");
    string s := "abc";

    Assert.ifTrue(s == "abc");
    console.write(".");
    string s2 := "de";
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde");
    console.write(".");

    Assert.ifTrue("s1" < "s2");
    Assert.ifFalse("s2" < "s2");
    Assert.ifFalse("s2" < "s1");

    console.writeLine(".");
}

// --- wideTests ---
wideTests()
{
    console.write("wideTests:");
    wide s := "abc"w;

    Assert.ifTrue(s == "abc"w);
    console.write(".");
    wide s2 := "de"w;
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde"w);
    console.write(".");

    Assert.ifTrue("s1"w < "s2"w);
    Assert.ifFalse("s2"w < "s2"w);
    Assert.ifFalse("s2"w < "s1"w);

    console.writeLine(".");
}

// --- shortTests ---

shortTests()
{
    console.write("shortTests:");
    short w := 1234;
    Assert.ifTrue(w == 1234);
    console.writeLine(".");
}

// --- conversionTests ---

conversionTests()
{
    console.write("conversionTests:");
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
   console.write("assignTests:");
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

struct BoxingFixedArrayStruct
{
   int  x;
   byte tmp[10];

   set(int n, int v)
   {
      tmp[n] := v;
   }

   set(int n, byte v)
   {
      tmp[n] := v;
   }

   test(int n, int v)
   {
      Assert.ifTrue(tmp[n] == v);
   }

   test(int n, byte d)
   {
      byte s := tmp[n];

      Assert.ifTrue(s == d);
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
   console.write("boxingTests:");

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
   console.write(".");

   BoxingFixedArrayStruct x := new BoxingFixedArrayStruct();
   byte b := 2;
   x.set(0,1);
   x.set(1,b);

   x.test(0,1);
   x.test(1,b);

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
   console.write("byRefTests:");

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

// --- inlineAssignmentTests ---
InlineAssignTesterBase
{
   x := 1;

   y := 2;

   validate()
   {
      Assert.ifTrue(x == 1);
      Assert.ifTrue(y == 2);
   }
}

InlineAssignTesterChild : InlineAssignTesterBase
{
}  

class InlineAssignTester2Base
{
   field x;
   field y; 

   validate()
   {
      Assert.ifTrue(x == 1);
      Assert.ifTrue(y == 2);
   }
}

inlineAssignmentTests()
{
   console.write("inlineAssignmentTests:");
   var b := new InlineAssignTesterBase();
   b.validate();
   console.write(".");
   var c := new InlineAssignTesterChild();
   c.validate();
   console.write(".");
   var child := new InlineAssignTester2Base
   {
      this x := 1;
      this y := 2;

      newMethod() {}
   };

   child.validate();
   console.writeLine(".");
}

class DispatchType
{
   _content;

   constructor(content)
   {
      _content := content;
   }

   get()
      = _content;
}


class DispatchTypeCompatible
{
   _dummy;
   _content;

   constructor(content)
   {
      _content := content;
   }   

   DispatchType cast()
      = new DispatchType(_content);
}

sealed class SingleDispatcher
{
   check(DispatchType arg)
   {
      ^ arg.get();
   }   
}

public singleDispatchTest()
{
   console.write("singleDispatchTest:");

   var weakDispatcher := new SingleDispatcher();

   auto arg1 := new DispatchType(1);
   auto arg2 := new DispatchTypeCompatible(2);

   Assert.ifTrue(weakDispatcher.check(arg1) == 1);
   Assert.ifTrue(weakDispatcher.check(arg2) == 2);
   console.write(".");

   auto dispatcher := new SingleDispatcher();

   Assert.ifTrue(dispatcher.check(arg1) == 1);
   Assert.ifTrue(dispatcher.check(arg2) == 2);
   console.writeLine(".");
}

sealed class VariadicTester
{
    bool test1Passed := false;
    
    testTwoArgs(params int[] args)
    {
        int length := args.Length;

        test1Passed := length == 2;
    }

    validate()
    {
       Assert.ifTrue(test1Passed);
    }
}

variadicTest()
{
   console.write("variadicTest:");

   auto strong := new VariadicTester();
   strong.testTwoArgs(2,3);
   strong.validate();
   console.writeLine(".");
}


// --- dynamicLoadTests ---

public class DynamicallyLoadedClass
{
   constructor new() {}
}

dynamicLoadTests()
{
   console.write("dynamicLoadTests:");

   var class := new ClassReference("system_tests'DynamicallyLoadedClass");
   
   var o := class.new();

   Assert.ifTrue(o.instanceOf(DynamicallyLoadedClass));

   console.writeLine(".");
}   
