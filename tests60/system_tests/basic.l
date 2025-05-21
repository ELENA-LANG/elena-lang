import extensions;
import system'routines;
import system'dynamic;
import system'dynamic'expressions;
import system'text;
import system'text'parsing;
import system'io;
import ltests;

// --- ControlTest ---

public controlTest() : testCase()
{
    var b1 := false;
    if (true)
    {
        b1 := true
    };
    
    var b2 := nil;
    if (false)
    {
        b2 := false;
    }
    else
    {
        b2 := true
    };
    
    Assert.ifTrue(b1);    
    Assert.ifTrue(b2);    
    Console.write(".");

    bool b3 := true;
    Assert.ifEqual(b3 ? 2.0 : 3.0, 2.0);

    Console.write(".")
}

public objectTest() : testCase()
{
    Console.write("objectTest:");
    
    var a := 1;
    var b := "abc";
    
    Assert.ifTrue(a.equalReference(a) && a.equalReference(b).Inverted);
    
    Console.write(".")
}

// --- BooleanTest ---

public booleanTest() : testCase()
{
    var b1 := true;
    var b2 := false;
    
    Assert.ifTrue(b1 == b1);
    Assert.ifFalse(b1 == b2);
    Assert.ifFalse(b2 == b1);
    Assert.ifTrue(b2 == b2);
    Assert.ifFalse(b2 != b2);
    Assert.ifTrue(b1 != b2);
    
    Console.write(".")
}

// --- intTests ---

// --- intTests ---

public byteTests() : testCase()
{
   byte n := 2;
   byte m := 3;
   byte k := 4;
   byte l := 3;
                    
   Assert.ifTrue(2 + 3 * 4 == 14);
   Assert.ifTrue(n + m * k == 14);
   Console.write(".");
   Assert.ifFalse(n == m);
   Assert.ifTrue(n == n);
   Assert.ifTrue(n < m);
   Assert.ifFalse(n > m);
   Assert.ifTrue(n <= m);
   Assert.ifTrue(n <= n);
   Assert.ifTrue(n >= n);
   Assert.ifFalse(n >= m);
   Assert.ifFalse(m > l);
   Assert.ifFalse(m < l);
   Assert.ifTrue(m >= l);
   Assert.ifTrue(m <= l);
   Console.write(".");

   m := n + 3;
   Assert.ifEqual(m, 5);
   n := m - 2;
   Assert.ifEqual(n, 3);
   m := n * 4;
   Assert.ifEqual(m, 12);
   m := m / 3;
   Assert.ifEqual(m, 4);
   Console.write(".");

   n := 12;
   n := n shr:2;
   Assert.ifEqual(n, 3);
   n := 12;
   n := n shl:2;
   Assert.ifEqual(n, 48);
   Console.write(".");
}

public intTests() : testCase()
{
   int n := 2;
   int m := 3;
   int k := 4;
   int l := 3;
                    
   Assert.ifTrue(2 + 3 * 4 == 14);
   Assert.ifTrue(n + m * k == 14);
   Console.write(".");
   Assert.ifFalse(n == m);
   Assert.ifTrue(n == n);
   Assert.ifTrue(n < m);
   Assert.ifFalse(n > m);
   Assert.ifTrue(n <= m);
   Assert.ifTrue(n <= n);
   Assert.ifTrue(n >= n);
   Assert.ifFalse(n >= m);
   Assert.ifFalse(m > l);
   Assert.ifFalse(m < l);
   Assert.ifTrue(m >= l);
   Assert.ifTrue(m <= l);
   Console.write(".");

   int m2 := 10;
   int k2 := -2;
   int l2 := -10;

   Assert.ifFalse(k2 > m2);
   Assert.ifTrue(m2 > k2);
   Assert.ifFalse(k2 < l2);
   Console.write(".");
   
   int i := 1;
   i += 2;
   i -= 2;
   i *= 2;
   i /= 2;    
   Assert.ifTrue(i == 1);
   Console.write(".");

   n := IntNumber.MinValue;
   string s := n.toPrintable();
   Assert.ifTrue(s == "-2147483648");
   Console.write(".");
}

public uintTests() : testCase()
{
   uint k := 2H;
   uint l := 0AH;
   uint m := 081000000H;
   uint n := 08FFFFFFFH;

   Assert.ifFalse(k > l);  // 2 > 10
   Assert.ifFalse(m < l);  // 081000000H < 10
   Assert.ifTrue(l < m);   // 10 < 081000000H
   Assert.ifTrue(n > m);  // 08FFFFFFFH > 081000000H
   Console.write(".");

   m := 80000000H;
   n := 10H;
   k := m / n;
   Assert.ifTrue(k == 8000000H);
   Console.write(".");
}


// --- longTests ---

public longTests() : testCase()
{
    long l1 := 100000000000000l;
    long l2 := 5000000000000l;
    
    var l3 := l1 / l2;
    Assert.ifTrue(l3 == 20);
    
    int n32 := 23456;
    long n64 := n32;
    int in32 := -23458;
    long in64 := in32;
    Assert.ifTrue(n64 == 23456l);
    Assert.ifTrue(in64 == -23458l);
    Console.write(".");

    long n := 2l;
    long m := 3l;
    long k := 4l;
                 
    Assert.ifTrue(2l + 3l * 4l == 14l);
    Assert.ifTrue(n + m * k == 14l);
    Console.write(".");

    n := 2000000l;
    m := 500000l;    
    Assert.ifTrue(n * m == 1000000000000l);
    Console.write(".");

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
    Console.write(".");

    long l := 2l;
    l2 := -2l;
    Assert.ifTrue(l == l);   
    Assert.ifTrue(l2 == l2); 
    Assert.ifFalse(l2 == l); 
    Assert.ifFalse(l == l2); 
    Assert.ifFalse(l < l2);  
    Assert.ifTrue(l > l2);   
    Assert.ifTrue(l2 < l);   
    Assert.ifFalse(l2 > l);  
    Console.write(".");

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
    Console.write(".");

    long i := 0l;
    i += 3l;
    i -= 2l;
    i *= 2l;
    i /= 2l;
    Assert.ifTrue(i == 1l);
    Console.write(".");

    var tester := (long l, long control1, long control2) { long r1 := -l; long r2 := -r1; Assert.ifTrue(r1 == control1); Assert.ifTrue(r2 == control2);  };

    tester(-1l,1l,-1l);
    tester(-5000000000l,5000000000l,-5000000000l);
    Console.write(".");
}

// --- realTests ---

public realTests() : testCase()
{
   real r := 2.89787;
   Assert.ifFalse(r == 2.8);
   Assert.ifTrue(r == 2.89787);
   Console.write(".");

   real n := 2.0;
   real m := 3.0;
   real k := 4.0;

   Assert.ifTrue(n + m * k == 14.0);
   Console.write(".");

   real i := 1.0;
   i += 2.1;
   i -= 2.1;
   i *= 2.0;
   i /= 2.0;
   Assert.ifTrue(i == 1.0);
   Console.write(".");
    
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
   Console.write(".");

   int rounded := r.RoundedInt;
   Assert.ifTrue(rounded == 3);
   Console.write(".");

   real negr := -2.3;
   real absr := negr.Absolute;
   Assert.ifTrue(absr == 2.3);
   Console.write(".");
}

// --- gcTests ---

public gcTests() : testCase()
{
   for (int i := 0; i < 100000; i := i + 1)
   {
      new IntNumber();
   };
   Console.write(".");
}

// --- stringTests ---
public stringTests() : testCase()
{
    Assert.ifTrue("abc" == "abc");
    Assert.ifFalse("ab" == "abc");
    Assert.ifTrue("ba" > "ab");
    Assert.ifTrue("ab" < "abc");
    Assert.ifTrue("abc" + "de" == "abcde");
    Console.write(".");

    Assert.ifTrue("abcd".indexOf(0, "bc")==1);
    Assert.ifTrue("abcd".indexOf(2, "bc")==-1);

    Assert.ifTrue("acd".insert(1,"b") == "abcd");
    Assert.ifTrue("abecd".delete(2,1) == "abcd");
    Assert.ifTrue("abcd".Substring(1,2) == "bc");
    Assert.ifTrue("abc"[1] == "b");
    Console.write(".");

    Assert.ifFailed({ "abc"[-1] });
    Assert.ifFailed({ "abc"[4] });
    Console.write(".");

    string s := "abc";

    Assert.ifTrue(s == "abc");
    Console.write(".");
    string s2 := "de";
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde");
    Console.write(".");

    Assert.ifTrue("s1" < "s2");
    Assert.ifTrue("s2" < "s2");
    Assert.ifFalse("s2" < "s1");

    Console.write(".");
}

// --- wideTests ---
public wideTests() : testCase()
{
    Assert.ifTrue("abc"w == "abc"w);
    Assert.ifFalse("ab"w == "abc"w);
    Assert.ifTrue("ba"w > "ab"w);
    Assert.ifTrue("ab"w < "abc"w);
    Assert.ifTrue("abc"w + "de"w == "abcde"w);

    Assert.ifTrue("abcd"w.indexOf(0, "bc"w)==1);
    Assert.ifTrue("abcd"w.indexOf(2, "bc"w)==-1);

    Assert.ifTrue("acd"w.insert(1,"b"w) == "abcd"w);
    Assert.ifTrue("abecd"w.delete(2,1) == "abcd"w);
    Assert.ifTrue("abcd"w.Substring(1,2) == "bc"w);
    Assert.ifTrue("abc"w[1] == "b");

    Assert.ifFailed({ "abc"w[-1] });
    Assert.ifFailed({ "abc"w[4] });
    Console.write(".");

    wide s := "abc"w;

    Assert.ifTrue(s == "abc"w);
    Console.write(".");
    wide s2 := "de"w;
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde"w);
    Console.write(".");

    Assert.ifTrue("s1"w < "s2"w);
    Assert.ifTrue("s2"w < "s2"w);
    Assert.ifFalse("s2"w < "s1"w);

    Console.write(".");
}

// --- shortTests ---

public shortTests() : testCase()
{
    short w := 1234;
    Assert.ifTrue(w == 1234);
    Console.write(".");

    short n := 2;
    short m := 3;
    short k := 4;
    short l := 3;
                    
    Assert.ifTrue(n + m * k == 14);
    Console.write(".");
}

// --- ushortTests ---

public ushortTests() : testCase()
{
    short w := 1234;
    Assert.ifTrue(w == 1234);
    Console.write(".");

    ushort n := 2;
    ushort m := 3;
    ushort k := 4;
    ushort l := 3;

    Assert.ifTrue(n < m);
    Assert.ifFalse(n > m);
    Assert.ifTrue(n <= m);
    Assert.ifTrue(n <= n);
    Assert.ifTrue(n >= n);
    Assert.ifFalse(n >= m);
    Assert.ifTrue(n == n);
    Assert.ifFalse(n == m);
                    
    Console.write(".");

    Assert.ifTrue(n + m * k == 14);
    Console.write(".");
}

// --- conversionTests ---

public conversionTests() : testCase()
{
    var n := "23".toInt();
    var sn := n.toString();
    Assert.ifTrue(sn == "23");
    Console.write(".");

    Assert.ifTrue("1234567890123456".toLong() == 1234567890123456l);
    Assert.ifTrue("-123".toInt() == -123);
    Assert.ifTrue("-1234567890123456".toLong() == -1234567890123456l);
    Console.write(".");

    Assert.ifTrue("23".toReal() == 23.0);
    Assert.ifTrue("123.456789".toReal() == 123.456789);
    Assert.ifTrue("-123.456789".toReal() == -123.456789);
    Console.write(".");

    Assert.ifTrue(-2l.toInt() == -2);
    Assert.ifTrue(-2.toLong() == -2l);
    
    Assert.ifTrue(-2.0.toInt() == -2);
    Assert.ifTrue(-2.toReal() == -2.0);
        
    Assert.ifTrue(-2.0.toLong() == -2l);
    Assert.ifTrue(-2l.toReal() == -2.0);
    Console.write(".");

    int n2 := 1234;
    short w := n2;
    Assert.ifTrue(w == 1234);
    Console.write(".");
    string s := n2.toString();
    Assert.ifTrue(s == "1234");
    Console.write(".");
    int n3 := s.toInt();
    Assert.ifTrue(n3 == n2);
    Console.write(".");
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

public assignTests() : testCase()
{
   var o := new AssignTestClass(2,"test");
   o.test(2,"test");

   Console.write(".");
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

struct XInteger
{
   int x;

   constructor(int x)
   {
      this x := x;
   }

   int cast()
      = x;
}

extension boxOp : IntNumber
{
   bool check(object arg)
      = arg == self;
}


public boxingTests() : testCase()
{
   int n := 123;

   // --- boxing read-only struct
   // 1a. passing directly
   BoxingProber.check(n);
   Console.write(".");

   // 1b. weak passing directly
   var o := BoxingProber;
   o.check(n);
   Console.write(".");

   // --- boxing / unboxing variable struct
   BoxingStruct st := default;
   BoxingProber.set(st);
   st.test(3,4);
   Console.write(".");

   BoxingStruct st2;
   var p := BoxingProber;
   p.set(st2);
   st2.test(3,4);
   Console.write(".");

   BoxingFixedArrayStruct x := new BoxingFixedArrayStruct();
   byte b := 2;
   x.set(0,1);
   x.set(1,b);

   x.test(0,1);
   x.test(1,b);
   Console.write(".");

   var xint := new XInteger(2);

   var nint := cast int(xint);

   Assert.ifTrue(nint.instanceOf(IntNumber));
   Console.write(".");

   int n2 := 2;
   int m2 := 2;

   Assert.ifTrue(n2.check(m2));
   Console.write(".");
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

public byRefTests() : testCase()
{
   int n := 0;
   ByRefProber.set(ref n);

   Assert.ifTrue(n == 123);
   Console.write(".");

   int m := 0;
   var o := ByRefProber;
   o.set(ref m);

   Assert.ifTrue(m == 123);
   Console.write(".");

   string s := "none";
   ByRefProber.set(ref s);

   Assert.ifTrue(s == "hello");
   Console.write(".");

   string s2 := "none";
   o.set(ref s2);

   Assert.ifTrue(s2 == "hello");
   Console.write(".");
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

public inlineAssignmentTests() : testCase()
{
   var b := new InlineAssignTesterBase();
   b.validate();
   Console.write(".");
   var c := new InlineAssignTesterChild();
   c.validate();
   Console.write(".");
   var child := new InlineAssignTester2Base
   {
      this x := 1;
      this y := 2;

      newMethod() {}
   };

   child.validate();
   Console.write(".");
}

class DispatchType
{
   _content;

   constructor(int content)
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

   constructor(int content)
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

public singleDispatchTest() : testCase()
{
   var weakDispatcher := new SingleDispatcher();

   auto arg1 := new DispatchType(1);
   auto arg2 := new DispatchTypeCompatible(2);

   Assert.ifTrue(weakDispatcher.check(arg1) == 1);
   Assert.ifTrue(weakDispatcher.check(arg2) == 2);
   Console.write(".");

   auto dispatcher := new SingleDispatcher();

   Assert.ifTrue(dispatcher.check(arg1) == 1);
   Assert.ifTrue(dispatcher.check(arg2) == 2);
   Console.write(".");
}


sealed class VariadicTester
{
   bool test1Called := false;
   bool test2Called := false;
    
   test1(params int[] others)
   {
      test1Called := true
   }
    
   test2(string s, params int[] others)
   {
      test2Called := true
   }    

   int sum(params int[] numbers)
   {
      int sum := 0;
      for (int i := 0; i < numbers.Length; i += 1) {
         sum += numbers[i];
      };

      ^ sum;
   }

   int passAndSum(params int[] numbers)
      <= sum(params numbers);
    
   validate()
   {
      Assert.ifTrue(test1Called).ifTrue(test2Called)
   }
}

public variadicTest() : testCase()
{
    // run-time
    var tester := new VariadicTester();
    tester.test1(1,3,3);
    tester.test2("hello",3,3);
    
    tester.validate();
    Console.write(".");

    Assert.ifTrue(tester.sum(1,2,3)==6);
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
    Console.write(".");
    
    // compile-time
    auto tester2 := new VariadicTester();

    tester2.test1(1,3,3);
    tester2.test2("hello",3,3);    
    tester2.validate();
    Console.write(".");

    Assert.ifTrue(tester.sum(1,2,3)==6);
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
    
    Console.write(".")
}

// --- dynamicLoadTests ---

public class DynamicallyLoadedClass
{
   constructor new() {}
}

public dynamicLoadTests() : testCase()
{
   var class := new ClassReference("system_tests'DynamicallyLoadedClass");
   
   var o := class.new();

   Assert.ifTrue(o.instanceOf(DynamicallyLoadedClass));

   Console.write(".");
}   

interface ITest
{
   abstract int Index
   {
      get();
   }
}

class ATest
{
   int value;

   constructor(int value)
   {
      this value := value
   }

   get int Index()
   {
      ^ value;
   }

   ITest cast() = new ITest { embeddable dispatch() => self; };
}

public interfaceTests() : testCase()
{
   var a := new ATest(2);

   ITest i := a;

   int n := i.Index;
   Assert.ifTrue(n == 2);

   Console.write(".");
}

singleton VariadicFunctionDispatchTester
{
   function(params int[] args)
   {
      int l := args.Length;
      int sum := 0;
      for (int i := 0; i < l; i += 1) {
         sum := sum + args[i]
      };

      ^ sum
   }
}

singleton VariadicDispatchTester
{
   product(params int[] args)
   {
      int l := args.Length;
      int prod := 1;
      for (int i := 0; i < l; i += 1) {
         prod := prod * args[i]
      };

      ^ prod
   }
}

public vardispatcherTest() : testCase()
{
   var r1 := VariadicFunctionDispatchTester(1,2,3);
   Assert.ifTrue(r1 == 6);
   var r2 := VariadicDispatchTester.product(1,2,3,4);
   Assert.ifTrue(r2 == 24);
   Console.write(".");

   var o := VariadicFunctionDispatchTester;
   var o2 := VariadicDispatchTester;
   var r3 := o(1,2,3);
   Assert.ifTrue(r3 == 6);
   var r4 := o2.product(1,2,3,4);
   Assert.ifTrue(r4 == 24);

   Console.write(".");
}

// --- mssgDispatchTest ---

singleton DispatchTester
{
    mssgTest(x,y)
    {
        ^ x + y
    }
}

public mssgDispatchTest() : testCase()
{
    var mssg := mssg mssgTest[3];
    var r := mssg(DispatchTester, 1, 2);
    
    Assert.ifTrue(r == 3);
    
    Console.write(".")
}

// --- MultySelectTest ---

public multySelectTest() : testCase()
{
    var n := 3;
    
    n =>
        1 : { Assert.ifTrue(n==1); }
        2 : { Assert.ifTrue(n==2); }
        3 : { Assert.ifTrue(n==3); };
        
    Console.write(".")        
}

// --- stringEnumerationTest ---

public stringEnumerationTest() : testCase()
{
    var s := String.MinValue;
    "123".forEach::(x)
    {
        s := x.toPrintable() + s;
    };
    
    Assert.ifTrue(s == "321");
        
    Console.write(".")        
}

// --- TextBufferTest ---

public textBufferTest() : testCase()
{
    var buffer := new system'text'TextBuilder();
    
    buffer.write("a");
    buffer.write("b");
    buffer.write("c");

    Assert.ifTrue(buffer.Value == "abc");

    Console.write(".");
}

// --- LoopTest ---

LoopTester
{
   int x := 5;

   bool next()
   {
      x -= 1;

      ^ x > 0;
   }

   assert()
   {
      Assert.ifTrue(x == 0)
   }
}

public loopTest() : testCase()
{
    int sum := 0;
    for(int i := 0; i < 5; i += 1)
    {
        sum += i;
    };
    
    Assert.ifTrue(sum == 10);
    
    int i := 5;
    while (i > 0)
    {
        i -= 1;
        
        sum := sum - i;         
    };
    
    Assert.ifTrue(sum == 0);
    
    Console.write(".");

    var counter := new LoopTester();
    while(counter.next()) {};
    counter.assert();

    Console.write(".")
}

// --- Action Test ---

singleton ActionTester
{
    testWeak(Func1 f, arg)
        = f(arg);
    
    testStrong(Func<int,int,int> f, int arg1, int arg2)
        = f(arg1, arg2);
}

public actionTest() : testCase()
{
    var r := ActionTester.testWeak((x => x + 2), 1);
    Assert.ifTrue(r == 3);
    
    var r2 := ActionTester.testStrong((int x, int y => x + y), 2, 3);
    Assert.ifTrue(r2 == 5);

    Console.write(".");
}

// --- ArrayTest ---

public arrayTest() : testCase()
{
    var a1 := new object[]{1,2,3};
    Assert.ifTrue(a1[1] == 2);

    Array a4 := Array.allocate(3);
    a4[0] := 2;
    a4[1] := 1;
    a4[2] := 3;

    Assert.ifTrue(a4[0] == 2);
                            
    Assert.ifFailed({ "abc"[-1] });
    Assert.ifFailed({ "abc"[4] });

    Console.write(".")    
}

// --- ExceptionTest ---

class MyException : Exception
{
    constructor new(s)
        <= super new(s);
}

class MyException2 : Exception
{
    constructor new(s)
        <= super new(s);
}

singleton ExecTester
{
    break()
    {
        MyException.new("OnError handler works").raise()
    }
    
    break2()
    {
        MyException2.new("OnError2 handler works").raise()
    }
    
    break3()
    {
        MyException2.new("Nested OnError2 handler works").raise()
    }
}

public exceptionTest() : testCase()
{
    bool raised := false;
    ExecTester.break() \\ on::(e){ raised := true };
    Assert.ifTrue(raised);
    Console.write(".");
    
    bool passed := false;
    try
    {
        ExecTester.break2()
    }
    catch::
    {
        function(MyException e)
        {
            Assert.ifTrue(false)
        }
        function(MyException2 e)
        {
            passed := true;
        }        
    };
    Assert.ifTrue(passed);
    Console.write(".");
    
    bool passed2 := false;
    try
    {
        try
        {
            MyException2.new("Nested OnError2 handler works").raise()
        }
        catch(MyException e)
        {
            Console.writeLine("!!Wrong handler");
            Assert.ifTrue(false)
        }
    }
    catch(MyException2 e)
    {
        passed2 := true;
    };
    Assert.ifTrue(passed2);
    
    Console.write(".")
}

// -- tryFinallyTest ---

public tryFinallyTest() : testCase()
{
   var passed := false;
   var passed2 := false;
   Assert.ifFailed(
   {
      var o := new Object();
      try
      {
         o.fail()
      }
      finally
      {
         passed := true
      }
   });
   Assert.ifTrue(passed);    
   Console.write(".");

   Assert.ifNotFailed(
   {
      var o := new Object();
      try
      {
         o.toPrintable()
      }
      finally
      {
         passed2 := true
      }
   });
   Assert.ifTrue(passed);    
   Console.write(".");
}

// --- nestedFieldsTest --

public nestedFieldsTest() : testCase()
{
    var o := new
    {
        field3 := 3;
        
        innerMost() => new
        {
            field2 := 2;
            
            innerMost() => new
            {
                field1 := 1;
                
                innerMost() = this self;
                
                getField() = field1;
                
                getParentField() = field2;
                
                getParentParentField() = field3;
            };
        };
    };
  
    var n := o.innerMost();
    Assert.ifEqual(n.getField(), 1);
    Assert.ifEqual(n.getParentField(), 2);
    Assert.ifEqual(n.getParentParentField(), 3);
    
    Console.write(".")
}

public shortCircuitBooleanOperation() : testCase()
{
    int n := 0;
    var b := (n != 0) && (1/n != 0);
        
    Assert.ifFalse(b);
    
    Console.write(".")
}

class NestedTester
{
    int param1;
    int param2;
    
    constructor()
    {
        param1 := 1;
        param2 := 2;
    } 
    
    validate()
    {
        var c := { ^ param2 };        
        
        Assert.ifTrue(c() == param2);
    }
}

public nestedTest() : testCase()
{
    auto tester := new NestedTester();
        
    tester.validate();
    
    Console.write(".")
}

// --- nestetNestedTest() ---

class NestetNestedTester
{
    int param1;
    int param2;
    
    constructor()
    {
        param1 := 1;
        param2 := 2;
    } 
    
    validate()
    {
        var c := { ^ { ^ param2 } };        
        var cc := c();
        
        Assert.ifTrue(cc() == param2);
    }
}


public nestetNestedTest() : testCase()
{
    auto tester := new NestetNestedTester();
        
    tester.validate();
    
    Console.write(".")
}

public ifExpressionTest() : testCase()
{
    auto c := DynamicSingleton.load(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      Expression.If(                      
                          Expression.MessageCall(
                            new Message("notequal[2]"),
                            Expression.Variable("a"),
                            Expression.Variable("b")
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(true) 
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(false) 
                          )
                      )
                   ),
                   ScopeIdentifier.Variable("a"),
                   ScopeIdentifier.Variable("b")
                )
             );

    var o := c.compile();

    Assert.ifTrue(o.eval(1,2));
    Assert.ifFalse(o.eval(1,1));

    Console.write(".")    
}

public ifNilExpressionTest() : testCase()
{
    auto c := DynamicSingleton.load(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      Expression.If(                      
                          new CondMessageCallExpression(
                            Expression.Variable("a"),
                            "equal[2]",
                            Expression.Identifier("nil")
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(true) 
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(false) 
                          )
                      )
                   ),
                   ScopeIdentifier.Variable("a")
                )
             );

    var o := c.compile();

    Assert.ifTrue(o.eval(nil));
    Assert.ifFalse(o.eval(2));

    Console.write(".")   
}

public loopExpressionTest() : testCase()
{
    auto c := DynamicSingleton.load(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(      
                      Expression.DeclareAndAssigning(
                        "i",
                        Expression.Constant(0)
                      ),          
                      Expression.DeclareAndAssigning(
                        "k",
                        Expression.Constant(0)
                      ),          
                      Expression.Loop(                      
                          Expression.MessageCall(
                            new Message("less[2]"),
                            Expression.Variable("i"),
                            Expression.Variable("n")
                          ),                             
                          Expression.CodeBlock(
                            Expression.Assigning(
                                "k",
                                Expression.MessageCall(
                                    new Message("add[2]"),                          
                                    Expression.Variable("k"),
                                    Expression.Constant(2)
                                )
                            ), 
                            Expression.Assigning(
                                "i",
                                Expression.MessageCall(
                                    new Message("add[2]"),                          
                                    Expression.Variable("i"),
                                    Expression.Constant(1)
                                )
                            ) 
                          )                     
                      ),
                      Expression.Variable("k")
                   ),
                   ScopeIdentifier.Variable("n")
                )
             );

    var o := c.compile();

    Assert.ifEqual(o.eval(4), 8);
    Assert.ifEqual(o.eval(0), 0);

    Console.write(".")    
}

A;

class NilDispatcher
{
    int value := 0;
    
    testMe(int arg, A a)
    {
        value := arg + 1
    }
    
    testMe(int arg, object a)
    {
        value := arg
    }
    
    validate(int arg)
    {
        Assert.ifTrue(value == arg);
        
        value := 0
    }
}

public nilDispatcherTest() : testCase()
{
   auto d := new NilDispatcher();
   var d2 := d;
   A a := nil;
   var b := nil;
    
   d.testMe(1, a) \ back(nil);
   d.validate(2);

   d.testMe(1, b) \ back(nil);
   d.validate(1);
   Console.write(".");
    
   d.testMe(1, nil) \ back(nil);
   d.validate(1);
   Console.write(".");

   d2.testMe(1, a) \ back(nil);
   d2.validate(1);
    
   d2.testMe(1, b) \ back(nil);
   d2.validate(1);
    
   d2.testMe(1, nil) \ back(nil);
   d2.validate(1);

   Console.write(".")
}

public duplicateBoxing() : testCase()
{
    var t := 0;
    int t2 := 0;
    Console.then(
    {
        Assert.ifTrue(t == 0);
        Assert.ifTrue(t2 == 0);
        
        t := 1;
        t2 += 1;
    }).then(
    {
        Assert.ifTrue(t == 1);
        Assert.ifTrue(t2 == 1);
        
        t := 2;
        t2 += 1;
    });
    Console.write(".");

    Assert.ifTrue(t == 2);
    Assert.ifTrue(t2 == 2);
    Console.write(".");

    var mode := -1;
    var value := false;
    (value) ? { mode := 0 } ! { mode := 1 };
    
    Assert.ifTrue(mode == 1);
    Console.write(".");
    
    mode := -1;
    value := true;
    (value) ? { mode := 0 } ! { mode := 1 };
    
    Assert.ifTrue(mode == 0);

    Console.write(".")
}

public extension etOp
{
    testExtension(obj)
    {
        Assert.ifTrue(false)
    }
}
      
public extension etOp<T> 
{
    testExtension(A obj)
    {
        Assert.ifTrue(true)
    }
}
      
public templateExtensionTest() : testCase()
{
    auto a := new A();
    Assert.testExtension(a);
    
    Console.write(".")    
}  

public stackSafeExtensions() : testCase()
{
    int i := 5;
    var m := 5;
   
    int j := 2;
    var n := 2;
                         
    int r1 := i.mod(j);
    int r2 := i.mod(n);
    int r3 := m.mod(j);
    int r4 := m.mod(n);
    
    Assert.ifTrue(r1 == 1);
    Assert.ifTrue(r2 == 1);
    Assert.ifTrue(r3 == 1);
    Assert.ifTrue(r4 == 1);
    
    Console.write(".")
}

public genericTest() : testCase()
{
    var r := cast int(nilValue);
    Assert.ifTrue(r == nil);
    Console.write(".")
}


OuterFieldAssigner
{
   object target;

   apply(arg)
   {
      self.then({
         target := arg
      });
   }
   
   test(arg)
   {
      Assert.ifTrue(target == arg)
   }
}

public outerFieldAssigningTest() : testCase()
{
   var tester := new OuterFieldAssigner();

   tester.apply(2);
   tester.test(2);

   Console.write(".");
}

// --- methodNotFoundTest ---

public methodNotFoundTest() : testCase()
{
   var o := 2;
   var f := false;
   try
   {
      o.fail()
   }
   catch(MethodNotFoundException e)
   {
      f := true;
   };

   Assert.ifTrue(f);

   Console.write(".");
}

// --- methodNotFoundTest ---

public nilReferenceTest() : testCase()
{
   var o := nil;
   var f := false;
   try
   {
      o.fail()
   }
   catch(NilReferenceException e)
   {
      f := true;
   };

   Assert.ifTrue(f);

   Console.write(".");
}

// --- structTest ---

struct XY
{
   int x : prop;
   int y : prop;

   set(int x, int y)
   {
      this x := x;
      this y := y;
   }
   set(long r, long r2) {}

   constructor()
   {
      x := 2;
      y := 3;
   }
}

singleton StructTest
{
   testMe(XY r)
   {
      r.set(1,2);
   }

   testMe2(XY r)
   {
      var a := 3;
      var b := 4;

      r.set(a,b);
   }
}

public structTest() : testCase()
{
   XY xy := new XY();
   var xy2 := new XY();

   Assert.ifTrue(xy.x == 2);
   Assert.ifTrue(xy.y == 3);

   Console.write(".");

   StructTest.testMe(xy2);
   Assert.ifTrue(xy2.x == 1);
   Assert.ifTrue(xy2.y == 2);

   Console.write(".");

   StructTest.testMe2(xy2);
   Assert.ifTrue(xy2.x == 3);
   Assert.ifTrue(xy2.y == 4);

   Console.write(".");
}

singleton LookaheadHelper
{
   int X()
   {
      int r := 2;

      ^ r   
   }   

   WeakX()
   {
      ^ 3;
   }   
}

public lookaheadTests() : testCase()
{
   int r := LookaheadHelper.X() + 1;
   int r2 := LookaheadHelper.WeakX() + 1;

   Assert.ifTrue(r == 3);
   Assert.ifTrue(r2 == 4);

   Console.write(".");
}

struct IncTestStruct
{
   int x := 0;
   int y := 1;

   act()
   {
      x++;
      y--;
   }

   assert()
   {
      Assert.ifTrue(x == 1);
      Assert.ifTrue(y == 0);
   }
}

public incTest() : testCase()
{
   IncTestStruct struct := new IncTestStruct();

   struct.act();
   struct.assert();

   Console.write(".");
}


singleton MultiValGetter
{
   (object, object) getMultiVal()
   {
      ^ ("Alex", 13)
   }
}

public multiValTest() : testCase()
{
   var n := 2;
   var t := (n + 1,"abc");

   Assert.ifTrue(t.Item1 == 3);
   Assert.ifTrue(t.Item2 == "abc");

   Console.write(".");

   auto t2 := MultiValGetter.getMultiVal();

   Assert.ifTrue(t2.Item1 == "Alex");
   Assert.ifTrue(t2.Item2 == 13);

   Console.write(".");

   (var i1, var i2) := MultiValGetter.getMultiVal();
   Assert.ifTrue(i1 == "Alex");
   Assert.ifTrue(i2 == 13);

   Console.write(".");

   (string x1, int x2) := MultiValGetter.getMultiVal();
   Assert.ifTrue(x1 == "Alex");
   Assert.ifTrue(x2 == 13);

   Console.write(".");
}

// -- defaultTest --

DefaultA;

struct DefaultB
{
   int x := 0;

   internal int X() = x;

   bool equal(DefaultB val)
      = x == val.X();
}

struct DefaultC
{
   int y;

   internal int Y() = y;

   bool equal(DefaultC val)
      = y == val.Y();

   static DefaultC Default
      = DefaultC.load(0);

   constructor load(int v)
   {
      y := v
   }
}

public defaultTest() : testCase()
{
   DefaultA a := default;
   DefaultB b := default;
   DefaultC c := default;

   Assert.ifNil(a);
   Assert.ifEqual(b, new DefaultB());
   Assert.ifEqual(c, DefaultC.load(0));

   Console.write(".");
}

// -- retValTests --

RetValA
{
   eval() {}
}

extension RetValAOp : RetValA
{
   extendEval() {}
}

public retValTests() : testCase()
{
   auto a := new RetValA();

   Assert.ifInstanceOf(a.eval(), RetValA);
   Assert.ifInstanceOf(a.extendEval(), RetValA);

   Console.write(".");
}

static variableToBeLoaded = new Variable(1);

symbol preloader : preloaded
   = variableToBeLoaded.append(3);

public preloadedTests() : testCase()
{
   Assert.ifEqual(variableToBeLoaded, 4);

   Console.write(".");
}

// --- staticFieldAssigningOpTests ---

singleton StaticFieldAssigningOpHelper
{
   static int i := 0;

   inc()
   {
      i += 1;
   }

   get()
      = i;
}

public staticFieldAssigningOpTests() : testCase()
{
   StaticFieldAssigningOpHelper.inc();
   StaticFieldAssigningOpHelper.inc();

   Assert.ifEqual(StaticFieldAssigningOpHelper.get(), 2);
   Console.write(".");
}

public RealArrayOpTests() : testCase()
{
   real[] r := ::Array<real>.allocate(3);
   r[0] := 1.2;
   r[1] := 2.3;
   r[2] := 3.4;

   Assert.ifEqual(r.Length, 3);
   Assert.ifEqual(r[1], 2.3);

   Console.write(".");
}

public ShortArrayOpTests() : testCase()
{
   short[] r := ::Array<short>.allocate(3);
   r[0] := 1;
   r[1] := 2;
   r[2] := 3;

   Assert.ifEqual(r.Length, 3);
   Assert.ifEqual(r[1], 2);

   Console.write(".");
}

public ByteArrayOpTests() : testCase()
{
   byte[] r := ::Array<byte>.allocate(3);
   r[0] := 1;
   r[1] := 2;
   r[2] := 3;

   Assert.ifEqual(r.Length, 3);
   Assert.ifEqual(r[1], 2);

   Console.write(".");
}

public ArrayAssignOperation() : testCase()
{
   int temp[4];

   temp[0] := 1;
   temp[1] := 2;
   temp[2] := 3;
   temp[3] := 4;

   temp[0] += 2;
   temp[1] -= 1;
   temp[2] *= 2;
   temp[3] /= 2;

   Assert.ifEqual(temp[0], 3);
   Assert.ifEqual(temp[1], 1);
   Assert.ifEqual(temp[2], 6);
   Assert.ifEqual(temp[3], 2);

   Console.write(".");
}

VT_B;

VT_C
{
    VT_B cast()
        = new VT_B();
}

singleton VT_E
{
   load(params VT_B[] o)
   {
      ^ new TextBuilder().print(o.asEnumerable()).Value;
   }

   loadAndTypecast(params object[] args)
   {
      ^ self.load(cast VT_B(params args));
   }
}

public variadicTypecastingTests() : testCase()
{
   var b := new VT_B();
   var c := new VT_C();
   
   Assert.ifEqual(VT_E.loadAndTypecast(b,c), "system_tests'$private'VT_B,system_tests'$private'VT_B");

   Console.write(".");
}

II_A;

II_B
{
   II_A cast()
      = new II_A(); 
}

interface II_I
{
   abstract II_A retrieve();
}

II_O : interface<II_I>
{
   retrieve()
      = new II_B();
}

public interfaceImplTests() : testCase()
{
   var o := new II_O();

   II_I i := o;

   var r := i.retrieve();
   Assert.ifTrue(r is:II_A);
   Console.write(".");
}

interface IDecoratorA
{
   abstract giveMe();
}

class DecoratorBaseA : IDecoratorA
{
   field value;

   constructor(value)
   {
      this value := value
   }

   giveMe()
      = value;
}

class DecoratorA
{
   object target;
   object extra;

   constructor(target, extra)
   {
      this target := target;
      this extra := extra;
   }

   giveMe()
      = target.giveMe() + extra;

   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var proxy := ::{ dispatch() => self; };
      proxy.__injectInferface(type);
   
      ^ __received(proxy);
   }
}

public decoratorTest() : testCase()
{
   var a := new DecoratorBaseA(2);
   var decA := new DecoratorA(a, 3);

   Assert.ifEqual((decA as:IDecoratorA).giveMe(), 5);

   Console.write(".");
}

class ifNilOperatorHelper
{
   get()
     = 2;

   get Value()
     = 3;
}

public ifNilOperatorTest() : testCase()
{
   var a := new ifNilOperatorHelper();
   var b := nil;
   Assert.ifEqual(a?.get(), 2);
   Assert.ifEqual(a?.Value, 3);
   Console.write(".");
   Assert.ifNil(b?.get());
   Assert.ifNil(b?.Value);
   Console.write(".");
}

// --- enumTest ---

public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public enumTest() : testCase()
{
   Color red := Color.Red;
   Color green := Color.Green;

   Assert.ifTrue(red == Color.Red);
   Assert.ifFalse(red == Color.Green);
   Assert.ifEqual(red.toPrintable(), "Red");
   Assert.ifEqual(green.toPrintable(), "Green");

   Console.write(".");   
}

// --- passingNilArg ---

NilArgB;

NilArgA
{
   NilArgB b;

   constructor new(NilArgB? b)
   {
      this b := b
   }
}

NilArgC
{
   NilArgB b;

   constructor new(NilArgB b)
   {
      this b := b
   }
}

singleton NilArgTester<T>
{
   runWeak()
   {
      var b := nil;
      var t := class T;
      var a := t.new(b);
   }
   runStrong()
   {
      var b := nil;
      var a := class T.new(b);      
   }
}

public passingNilArg() : testCase()
{
   NilArgTester<NilArgA>.runWeak();
   Console.write(".");
   NilArgTester<NilArgA>.runStrong();
   Console.write(".");

   Assert.ifFailed({ NilArgTester<NilArgC>.runWeak() });
   Console.write(".");
   Assert.ifFailed({ NilArgTester<NilArgC>.runStrong() });
   Console.write(".");
}

public stringInterpolation() : testCase()
{
   var s := $"a_{ 1 }_b_{ 2 }_c";

   Assert.ifEqual(s,"a_1_b_2_c");
   Console.write(".");

   var s2 := $"{ 3 }_b_{ 4 }_c";

   Assert.ifEqual(s2,"3_b_4_c");
   Console.write(".");
}

// --- iteratorMethodTest ---

singleton IteratorEnumerable : Enumerable
{
   Enumerator enumerator()
      = IteratorEnumerable.infinitEnumerator();

   yield Enumerator infinitEnumerator()
   {
      :yield 2;
      :yield 5;
      :yield 7;
   }
}

public iteratorMethodTest() : testCase()
{
   auto e := IteratorEnumerable.enumerator();

   Assert.ifTrue(e.next());
   Assert.ifTrue(*e == 2);
   Assert.ifTrue(e.next());
   Assert.ifTrue(*e == 5);
   Assert.ifTrue(e.next());
   Assert.ifTrue(*e == 7);
   Assert.ifFalse(e.next());
   Console.write(".");
}

public isNilOperatorTest() : testCase()
{
   var a := nil; 
   var b := 2; 

   var r := a ?? b;
   Assert.ifTrue(r == 2);
   Console.write(".");

   Assert.ifNotFailed({ var a := new A(); var b := nil; var r := a ?? b.test() });
   Assert.ifFailed({ var a := nil; var b := nil; var r := a ?? b.test() });
   Console.write(".");

   string s := nil;
   Assert.ifEqual(s ?? String.MinValue, EmptyString);
   Console.write(".");
}

namespace IndexedTest
{
   A
   {
      indexed foo()
         = "from A";

      indexed foo(string s)
         = "from A with " + s;

      indexed foo(int n)
         = "from A with int";

      indexed internal fooInternal(string s)
         = "from A with hidden " + s;
   }
   
   B : A
   {
      indexed foo()
         = "from B";   

      indexed foo(string s)
         = "from B with "+s;   

      indexed internal fooInternal(string s)
         = "from B with hidden " + s;
   }   

   C : A
   {
      indexed bar()
         = "from C";   
   }
}

public indexedTests() : testCase()
{
   IndexedTest'A a := new IndexedTest'A();
   Assert.ifEqual(a.foo(), "from A");
   Assert.ifEqual(weak a.foo(), "from A");
   Console.write(".");

   IndexedTest'A b := new IndexedTest'B();
   Assert.ifEqual(b.foo(), "from B");
   Assert.ifEqual(weak b.foo(), "from B");

   IndexedTest'A c := new IndexedTest'C();
   Assert.ifEqual(c.foo(), "from A");
   Assert.ifEqual(weak c.foo(), "from A");
   Console.write(".");

   Assert.ifEqual(a.foo("Hello"), "from A with Hello");
   Assert.ifEqual(weak a.foo("Hello2"), "from A with Hello2");
   Assert.ifEqual(b.foo("Hello"), "from B with Hello");
   Assert.ifEqual(weak b.foo("Hello"), "from B with Hello");
   Assert.ifEqual(c.foo("Hello2"), "from A with Hello2");
   Assert.ifEqual(weak c.foo("Hello2"), "from A with Hello2");
   Console.write(".");

   Assert.ifEqual(a.fooInternal("Hello"), "from A with hidden Hello");
   Assert.ifEqual(b.fooInternal("Hello"), "from B with hidden Hello");
   Assert.ifEqual(c.fooInternal("Hello2"), "from A with hidden Hello2");

   Assert.ifFailed({weak a.fooInternal("Hello")});
   Assert.ifFailed({weak b.fooInternal("Hello")});
   Assert.ifFailed({weak c.fooInternal("Hello")});
   Console.write(".");

}

namespace sealedMethodDispatcherTest
{
   C;
   
   public closed A
   {
      sealed continueWith(C c)
      {
      }
   }
   
   public sealed B : A
   {
   }
}

public sealedMethodDispatcher() : testCase()
{
   sealedMethodDispatcherTest'B b := new sealedMethodDispatcherTest'B();
   var c := new sealedMethodDispatcherTest'C();
   
   Assert.ifNotFailed({ weak b.continueWith(c); });
   Console.write(".");
}

public objectOpTests() : testCase()
{
   A a := new A();

   string name := a.__getClassName();

   Assert.ifEqual(name, "system_tests'$private'A");
   Console.write(".");
}

namespace variadicBoxingTestset
{
   A
   {
      field args;

      constructor load(params string[] args)
      {
         this args := args;
      }

      int Length
         = args.Length;

      validate(params string[] args)
      {
         for (int i := 0; i < args.Length; i++) {
            Assert.ifEqual(this args[i], args[i]);
         }
      }

      static validate2(params string[] args)
      {
         string[] boxedArgs := args;

         Assert.ifEqual(boxedArgs.Length, args.Length);

         for (int i := 0; i < args.Length; i++) {
            Assert.ifEqual(boxedArgs[i], args[i]);
         }
      }
   }
}

public variadicBoxingTest() : testCase()
{
   auto a := variadicBoxingTestset'A.load("a", "b", "c");
   
   Assert.ifEqual(a.Length, 3);
   a.validate("a", "b", "c");
   Console.write(".");

   variadicBoxingTestset'A.validate2("a", "b", "c");
   Console.write(".");
}

namespace reflectionTestset
{
   A
   {
      string Name : prop;
   }
}


public reflectionTest() : testCase()
{
   reflectionTestset'A a := new reflectionTestset'A();

   MessageName subj := mssg Name;
   
   subj.setPropertyValue(a, "Ivan");

   Assert.ifEqual(subj.getPropertyValue(a), "Ivan");
   Console.write(".");   
}

namespace variadicFunctionTestset
{
   function variadicSum(params object[] args)
   { 
      var sum := 0;
      for (int i := 0; i < args.Length; i++) {
         sum := sum + args[i]
      };
   
      ^ sum
   }
   
   function sumAndTest(object sumToCheck, params object[] args)
   { 
      Assert.ifEqual(variadicSum(params args), sumToCheck);
   }
}


public variadicFunctionTest() : testCase()
{
   variadicFunctionTestset'sumAndTest(6, 1,2,3);
   Console.write(".");   
}

sealed class MyIntValue
{
   int value;

   constructor(string value)
   {
      this value := value.toInt()
   }

   int Value = value;
}

public implicitConvertorTest() : testCase()
{
   auto v := "2" as:MyIntValue;

   Assert.ifEqual(v.Value, 2);
   Console.write(".");   
}

public pointerTest() : testCase()
{
   pointer ptr := 2;

   var o := ptr;
   pointer ptr2 := o;

   int v := ptr.Value;
   int v2 := ptr.Value;

   Assert.ifEqual(v, 2);
   Assert.ifEqual(v2, 2);
   Console.write(".");
}

public regExTest() : testCase()
{
   auto regex1 := new RegEx("^a$");
   Assert.ifTrue(regex1.match("a"));
   Assert.ifFalse(regex1.match("aa"));
   Assert.ifFalse(regex1.match("b"));
   Console.write(".");

   auto regex2 := new RegEx("^[ab]$");
   Assert.ifTrue(regex2.match("a"));
   Assert.ifFalse(regex2.match("aa"));
   Assert.ifTrue(regex2.match("b"));
   Console.write(".");

   auto regex3 := new RegEx("^a*$");
   Assert.ifTrue(regex3.match("a"));
   Assert.ifTrue(regex3.match("aa"));
   Assert.ifTrue(regex3.match(""));
   Assert.ifFalse(regex3.match("b"));

   auto regex4 := new RegEx("^a+$");
   Assert.ifTrue(regex4.match("a"));
   Assert.ifTrue(regex4.match("aa"));
   Assert.ifFalse(regex4.match(""));
   Assert.ifFalse(regex4.match("b"));
   Console.write(".");

   auto regex5 := new RegEx("^[a-z]+$");
   Assert.ifTrue(regex5.match("abcd"));
   Assert.ifFalse(regex5.match("ab3cd"));
   Console.write(".");

   auto regex6 := new RegEx("^abb?a$");
   Assert.ifTrue(regex6.match("abba"));
   Assert.ifFalse(regex6.match("aca"));
   Assert.ifTrue(regex6.match("aba"));
   Console.write(".");

   auto regex7 := new RegEx("^(ht|f)$");
   Assert.ifTrue(regex7.match("ht"));
   Assert.ifTrue(regex7.match("f"));
   Assert.ifFalse(regex7.match("http"));
   Console.write(".");
}

public base64EncodingTest() : testCase()
{
   auto source := "Standard comparison operations are defined for all the primitive numeric types.";
   auto expected := "U3RhbmRhcmQgY29tcGFyaXNvbiBvcGVyYXRpb25zIGFyZSBkZWZpbmVkIGZvciBhbGwgdGhlIHByaW1pdGl2ZSBudW1lcmljIHR5cGVzLg==";
   
   auto buffer := MemoryBuffer.allocate();
   UTF8Encoding.fromString(source, buffer);
   var dest := Base64Encoding.toString(0, buffer.Length, *buffer);
   
   Assert.ifEqual(dest, expected);
   Console.write(".");
   
   buffer.clear();
   Base64Encoding.fromString(dest, buffer);
   
   var decoded := UTF8Encoding.toString(buffer);
   Assert.ifEqual(decoded, source);
   Console.write(".");
}

extension dummyOp
{
   testMe2(Func f)
      = self;
}

extension testOp<T> : T
{
   testMe(Func<T,T> f)
      = f(self);

   testMe2(Func<T,T> f)
      = f(self);
}

public directExtensionTemplateCall() : testCase()
{
   var r1 := 3::testMe<int>((x => x + 1));
   var r2 := 2::testMe2<int>((x => x + 1));

   Assert.ifEqual(r1, 4);
   Assert.ifEqual(r2, 3);

   Console.write(".");
}

// --- recordTest ---

MyRecord : record(string FirstName,string LastName,int Age);

public recordTest() : testCase()
{
   MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
   MyRecord r2 := new MyRecord("Ivan", "Ivanov", 22);
   MyRecord r3 := new MyRecord("Petr", "Ivanov", 22);

   Assert.ifTrue(r == r2);
   Assert.ifFalse(r == r3);

   Console.write(".");
}
