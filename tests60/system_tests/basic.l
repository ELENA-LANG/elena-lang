import extensions;
import system'routines;
import system'dynamic'expressions;

// --- ControlTest ---

controlTest()
{
    console.write("controlTest:");
    
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
    
    console.writeLine(".")
}

objectTest()
{
    console.write("objectTest:");
    
    var a := 1;
    var b := "abc";
    
    Assert.ifTrue(a.equalReference(a) && a.equalReference(b).Inverted);
    
    console.writeLine(".")
}

// --- BooleanTest ---

booleanTest()
{
    console.write("booleanTest:");
    
    var b1 := true;
    var b2 := false;
    
    Assert.ifTrue(b1 == b1);
    Assert.ifFalse(b1 == b2);
    Assert.ifFalse(b2 == b1);
    Assert.ifTrue(b2 == b2);
    Assert.ifFalse(b2 != b2);
    Assert.ifTrue(b1 != b2);
    
    console.writeLine(".")
}

// --- intTests ---

intTests()
{
   console.write("intTests:");

   int n := 2;
   int m := 3;
   int k := 4;
   int l := 3;
                    
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
   Assert.ifFalse(m > l);
   Assert.ifFalse(m < l);
   Assert.ifTrue(m >= l);
   Assert.ifTrue(m <= l);
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
    l2 := -2l;
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
    console.write(".");

    long i := 0l;
    i += 3l;
    i -= 2l;
    i *= 2l;
    i /= 2l;
    Assert.ifTrue(i == 1l);

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

    Assert.ifTrue("abc" == "abc");
    Assert.ifFalse("ab" == "abc");
    Assert.ifTrue("ba" > "ab");
    Assert.ifTrue("ab" < "abc");
    Assert.ifTrue("abc" + "de" == "abcde");
    console.write(".");

    Assert.ifTrue("abcd".indexOf(0, "bc")==1);
    Assert.ifTrue("abcd".indexOf(2, "bc")==-1);

    Assert.ifTrue("acd".insert(1,"b") == "abcd");
    Assert.ifTrue("abecd".delete(2,1) == "abcd");
    Assert.ifTrue("abcd".Substring(1,2) == "bc");
    Assert.ifTrue("abc"[1] == "b");
    console.write(".");

    Assert.ifFailed({ "abc"[-1] });
    Assert.ifFailed({ "abc"[4] });
    console.write(".");

    string s := "abc";

    Assert.ifTrue(s == "abc");
    console.write(".");
    string s2 := "de";
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde");
    console.write(".");

    Assert.ifTrue("s1" < "s2");
    Assert.ifTrue("s2" < "s2");
    Assert.ifFalse("s2" < "s1");

    console.writeLine(".");
}

// --- wideTests ---
wideTests()
{
    console.write("wideTests:");

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
    console.write(".");

    wide s := "abc"w;

    Assert.ifTrue(s == "abc"w);
    console.write(".");
    wide s2 := "de"w;
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde"w);
    console.write(".");

    Assert.ifTrue("s1"w < "s2"w);
    Assert.ifTrue("s2"w < "s2"w);
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

    var n := "23".toInt();
    var sn := n.toString();
    Assert.ifTrue(sn == "23");
    console.write(".");

    Assert.ifTrue("1234567890123456".toLong() == 1234567890123456l);
    Assert.ifTrue("-123".toInt() == -123);
    Assert.ifTrue("-1234567890123456".toLong() == -1234567890123456l);
    console.write(".");

    Assert.ifTrue("23".toReal() == 23.0);
    Assert.ifTrue("123.456789".toReal() == 123.456789);
    Assert.ifTrue("-123.456789".toReal() == -123.456789);
    console.write(".");

    Assert.ifTrue(-2l.toInt() == -2);
    Assert.ifTrue(-2.toLong() == -2l);
    
    Assert.ifTrue(-2.0.toInt() == -2);
    Assert.ifTrue(-2.toReal() == -2.0);
        
    Assert.ifTrue(-2.0.toLong() == -2l);
    Assert.ifTrue(-2l.toReal() == -2.0);
    console.write(".");

    int n2 := 1234;
    short w := n2;
    Assert.ifTrue(w == 1234);
    console.write(".");
    string s := n2.toString();
    Assert.ifTrue(s == "1234");
    console.write(".");
    int n3 := s.toInt();
    Assert.ifTrue(n3 == n2);
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
   console.write(".");

   var xint := new XInteger(2);

   var nint := cast int(xint);

   Assert.ifTrue(nint.instanceOf(IntNumber));
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
      for (int i := 0, i < numbers.Length, i += 1) {
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

variadicTest()
{
    console.write("variadicTest:");
    
    // run-time
    var tester := new VariadicTester();
    tester.test1(1,3,3);
    tester.test2("hello",3,3);
    
    tester.validate();
    console.write(".");

    Assert.ifTrue(tester.sum(1,2,3)==6);
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
    console.write(".");
    
    // compile-time
    auto tester2 := new VariadicTester();

    tester2.test1(1,3,3);
    tester2.test2("hello",3,3);    
    tester2.validate();
    console.write(".");

    Assert.ifTrue(tester.sum(1,2,3)==6);
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
    
    console.writeLine(".")
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

interfaceTests()
{
   console.write("interfaceTests:");

   var a := new ATest(2);

   ITest i := a;

   int n := i.Index;
   Assert.ifTrue(n == 2);

   console.writeLine(".");
}

singleton VariadicFunctionDispatchTester
{
   function(params int[] args)
   {
      int l := args.Length;
      int sum := 0;
      for (int i := 0, i < l, i += 1) {
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
      for (int i := 0, i < l, i += 1) {
         prod := prod * args[i]
      };

      ^ prod
   }
}

vardispatcherTest()
{
   console.write("vardispatcherTest:");

   var r1 := VariadicFunctionDispatchTester(1,2,3);
   Assert.ifTrue(r1 == 6);
   var r2 := VariadicDispatchTester.product(1,2,3,4);
   Assert.ifTrue(r2 == 24);
   console.write(".");

   var o := VariadicFunctionDispatchTester;
   var o2 := VariadicDispatchTester;
   var r3 := o(1,2,3);
   Assert.ifTrue(r3 == 6);
   var r4 := o2.product(1,2,3,4);
   Assert.ifTrue(r4 == 24);

   console.writeLine(".");
}

// --- mssgDispatchTest ---

singleton DispatchTester
{
    mssgTest(x,y)
    {
        ^ x + y
    }
}

mssgDispatchTest()
{
    console.write("mssgDispatchTest:");
    
    var mssg := mssg mssgTest[3];
    var r := mssg(DispatchTester, 1, 2);
    
    Assert.ifTrue(r == 3);
    
    console.writeLine(".")
}

// --- MultySelectTest ---

multySelectTest()
{
    console.write("mssgDispatchTest:");       
    
    var n := 3;
    
    n =>
        1 { Assert.ifTrue(n==1); }
        2 { Assert.ifTrue(n==2); }
        3 { Assert.ifTrue(n==3); };
        
    console.writeLine(".")        
}

// --- enumTest ---

enumTest()
{
    console.write("enumTest:");       
    
    var s := String.MinValue;
    "123".forEach:(x)
    {
        s := x.toPrintable() + s;
    };
    
    Assert.ifTrue(s == "321");
        
    console.writeLine(".")        
}

// --- TextBufferTest ---

textBufferTest()
{
    console.write("textBufferTest:");
    
    var buffer := new system'text'TextBuilder();
    
    buffer.write:"a";
    buffer.write:"b";
    buffer.write:"c";

    Assert.ifTrue(buffer.Value == "abc");

    console.writeLine(".");
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

loopTest()
{
    console.write("loopTest:");
    
    int sum := 0;
    for(int i := 0, i < 5, i += 1)
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
    
    console.write(".");

    var counter := new LoopTester();
    while(counter.next()) {};
    counter.assert();

    console.writeLine(".")
}

// --- Action Test ---

singleton ActionTester
{
    testWeak(Func1 f, arg)
        = f(arg);
    
    testStrong(Func<int,int,int> f, int arg1, int arg2)
        = f(arg1, arg2);
}

actionTest()
{
    console.write("actionTest:");
    
    var r := ActionTester.testWeak((x => x + 2), 1);
    Assert.ifTrue(r == 3);
    
    var r2 := ActionTester.testStrong((int x, int y => x + y), 2, 3);
    Assert.ifTrue(r2 == 5);

    console.writeLine(".");
}

// --- ArrayTest ---

arrayTest()
{
    console.write("arrayTest:");
    
    var a1 := new object[]{1,2,3};
    Assert.ifTrue(a1[1] == 2);

    Array a4 := Array.allocate(3);
    a4[0] := 2;
    a4[1] := 1;
    a4[2] := 3;

    Assert.ifTrue(a4[0] == 2);
                            
    Assert.ifFailed({ "abc"[-1] });
    Assert.ifFailed({ "abc"[4] });

    console.writeLine(".")    
}

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
        MyException.new:"OnError handler works".raise()
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

// --- ExceptionTest ---

exceptionTest()
{
    console.write("exceptionTest:");
    
    bool raised := false;
    ExecTester.break() \\ on:(e){ raised := true };
    Assert.ifTrue(raised);
    console.write(".");
    
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
    console.write(".");
    
    bool passed2 := false;
    try
    {
        try
        {
            MyException2.new("Nested OnError2 handler works").raise()
        }
        catch(MyException e)
        {
            console.writeLine:"!!Wrong handler";
            Assert.ifTrue(false)
        }
    }
    catch(MyException2 e)
    {
        passed2 := true;
    };
    Assert.ifTrue(passed2);
    
    console.writeLine(".")
}

// --- nestedFieldsTest --

nestedFieldsTest()
{
    console.write("nestedFieldsTest:");
    
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
    
    console.writeLine(".")
}

shortCircuitBooleanOperation()
{
    console.write("shortCircuitBooleanOperation:");
    
    int n := 0;
    var b := (n != 0) && (1/n != 0);
        
    Assert.ifFalse(b);
    
    console.writeLine(".")
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

nestedTest()
{
    console.write("nestedTest:");
    
    auto tester := new NestedTester();
        
    tester.validate();
    
    console.writeLine(".")
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


nestetNestedTest()
{
    console.write("nestetNestedTest:");
    
    auto tester := new NestetNestedTester();
        
    tester.validate();
    
    console.writeLine(".")
}

public ifExpressionTest()
{
    console.write("ifExpressionTest:");
    
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

    console.writeLine(".")    
}

public loopExpressionTest()
{
    console.write("loopExpressionTest:");
    
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

    console.writeLine(".")    
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

nilDispatcherTest()
{
   console.write("nilDispatcherTest:");
    
   auto d := new NilDispatcher();
   var d2 := d;
   A a := nil;
   var b := nil;
    
   d.testMe(1, a) \ back:nil;
   d.validate(2);

   d.testMe(1, b) \ back:nil;
   d.validate(1);
   console.write(".");
    
   d.testMe(1, nil) \ back:nil;
   d.validate(1);
   console.write(".");

   d2.testMe(1, a) \ back:nil;
   d2.validate(1);
    
   d2.testMe(1, b) \ back:nil;
   d2.validate(1);
    
   d2.testMe(1, nil) \ back:nil;
   d2.validate(1);

   console.writeLine(".")
}

duplicateBoxing()
{
    console.write("duplicateBoxing:");

    var t := 0;
    int t2 := 0;
    console.then(
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
    
    Assert.ifTrue(t == 2);
    Assert.ifTrue(t2 == 2);

//    var mode := -1;
//    var value := false;
//    value ? { mode := 0 } : { mode := 1 }  ;
//    
//    Assert.ifTrue(mode == 1);
//    
//    value := true;
//    value ? { mode := 0 } : { mode := 1 }  ;
//    
//    Assert.ifTrue(mode == 0);

    console.writeLine(".")
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
      
templateExtensionTest()
{
    console.write("templateExtensionTest:");
    
    auto a := new A();
    Assert.testExtension(a);
    
    console.writeLine(".")    
}  

public stackSafeExtensions()
{
    console.write("stackSafeExtensions:");
    
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
    
    console.writeLine(".")
}

genericTest()
{
    console.write("genericTest:");
    
    var r := cast int(nilValue);
    Assert.ifTrue(r == nil);
    console.writeLine(".")
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

outerFieldAssigningTest()
{
   console.write("outerFieldAssigningTest:");

   var tester := new OuterFieldAssigner();

   tester.apply(2);
   tester.test(2);

   console.writeLine(".");
}

// --- methodNotFoundTest ---

methodNotFoundTest()
{
   console.write("methodNotFoundTest:");

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

   console.writeLine(".");
}

// --- methodNotFoundTest ---

nilReferenceTest()
{
   console.write("nilReferenceTest:");

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

   console.writeLine(".");
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

structTest()
{
   console.write("structTest:");
   XY xy := new XY();
   var xy2 := new XY();

   Assert.ifTrue(xy.x == 2);
   Assert.ifTrue(xy.y == 3);

   console.write(".");

   StructTest.testMe(xy2);
   Assert.ifTrue(xy2.x == 1);
   Assert.ifTrue(xy2.y == 2);

   console.write(".");

   StructTest.testMe2(xy2);
   Assert.ifTrue(xy2.x == 3);
   Assert.ifTrue(xy2.y == 4);

   console.writeLine(".");
}
