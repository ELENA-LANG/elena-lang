import system'routines;
import system'dynamic'expressions;
import extensions;

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
    console.write("mssgDispatchTest..");
    
    var mssg := mssg mssgTest[3];
    var r := mssg(DispatchTester, 1, 2);
    
    Assert.ifTrue(r == 3);
    
    console.writeLine("done")
}

// --- MultySelectTest ---

multySelectTest()
{
    console.write("mssgDispatchTest..");       
    
    var n := 3;
    
    n =>
        1 { Assert.ifTrue(n==1); }
        2 { Assert.ifTrue(n==2); }
        3 { Assert.ifTrue(n==3); };
        
    console.writeLine("done")        
}

// --- enumTest ---

enumTest()
{
    console.write("enumTest..");       
    
    var s := String.MinValue;
    "123".forEach:(x)
    {
        console.write(x);
        
        s := x.toPrintable() + s;
    };
    
    Assert.ifTrue(s == "321");
        
    console.writeLine("done")        
}

class MyException : Exception
{
    constructor new(s)
        <= new(s);
}

class MyException2 : Exception
{
    constructor new(s)
        <= new(s);
}

singleton ExecTester
{
//    do : anAction [ anAction eval. ]
//
//    do:x with:y
//    [
//    ]
//            
//    run : anAction
//    [
//        anAction eval x:"Eval Works!!".
//    ]
    
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
    console.write("actionTest...");
    
    var r := ActionTester.testWeak((x => x + 2), 1);
    Assert.ifTrue(r == 3);
    
    var r2 := ActionTester.testStrong((int x, int y => x + y), 2, 3);
    Assert.ifTrue(r2 == 5);

    console.writeLine("done");
}

// --- ConvertorTest ---

convertorTest()
{
    console.write("convertorTest..");
        
    var n := "23".toInt();
    var sn := n.toString();
    Assert.ifTrue(sn == "23");
    
    Assert.ifTrue("1234567890123456".toLong() == 1234567890123456l);
    Assert.ifTrue("-123".toInt() == -123);
    Assert.ifTrue("-1234567890123456".toLong() == -1234567890123456l);

    Assert.ifTrue("23".toReal() == 23.0);
    Assert.ifTrue("123.456789".toReal() == 123.456789);
    Assert.ifTrue("-123.456789".toReal() == -123.456789);
    
    Assert.ifTrue(-2l.toInt() == -2);
    Assert.ifTrue(-2.toLong() == -2l);
    
    Assert.ifTrue(-2.0.toInt() == -2);
    Assert.ifTrue(-2.toReal() == -2.0);
        
    Assert.ifTrue(-2.0.toLong() == -2l);
    Assert.ifTrue(-2l.toReal() == -2.0);
    
    console.writeLine("done")        
}

// --- LoopTest ---

loopTest()
{
    console.write("loopTest..");
    
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
    
    console.writeLine("done")
}

// --- ExceptionTest ---

exceptionTest()
{
    console.write("exceptionTest..");
    
    bool raised := false;
    ExecTester.break() \\ on:(e){ raised := true };
    Assert.ifTrue(raised);
    
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
    
    console.writeLine("done")
}

// --- TextBufferTest ---

textBufferTest()
{
    console.write("textBufferTest...");
    
    var buffer := new system'text'TextBuilder();
    
    buffer.write:"a";
    buffer.write:"b";
    buffer.write:"c";

    Assert.ifTrue(*buffer == "abc");

    console.writeLine("done");
}

nestedFieldsTest()
{
    console.write("nestedFieldsTest..");
    
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
    
    console.writeLine("done")
}

shortCircuitBooleanOperation()
{
    console.write("shortCircuitBooleanOperation..");
    
    int n := 0;
    var b := (n != 0) && (1/n != 0);
        
    Assert.ifFalse(b);
    
    console.writeLine("done")
}

sealed class VariadicTested
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
    
    validate()
    {
        Assert.ifTrue(test1Called).ifTrue(test2Called)
    }
}


variadicTest()
{
    console.write("variadicTest..");
    
    // run-time
    var tester := new VariadicTested();
    tester.test1(1,3,3);
    tester.test2("hello",3,3);
    
    tester.validate();
    
    // compile-time
    auto tester2 := new VariadicTested();
    tester2.test1(1,3,3);
    tester2.test2("hello",3,3);
    
    tester2.validate();
    
    console.writeLine("done")
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
    console.write("nestedTest..");
    
    auto tester := new NestedTester();
        
    tester.validate();
    
    console.writeLine("done")
}

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
    console.write("nestetNestedTest..");
    
    auto tester := new NestetNestedTester();
        
    tester.validate();
    
    console.writeLine("done")
}

//duplicateStructBoxing()
//{
//    console.write("duplicateStructBoxing..");
//    
//    int mode := -1;
//    var value := false;
//    value ? { mode := 0 } : { mode := 1 }  ;
//    
//    Assert.ifTrue(mode == 1);
//    
//    console.writeLine("done")
//}

duplicateBoxing()
{
    console.write("duplicateBoxing..");

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

    var mode := -1;
    var value := false;
    value ? { mode := 0 } : { mode := 1 }  ;
    
    Assert.ifTrue(mode == 1);
    
    value := true;
    value ? { mode := 0 } : { mode := 1 }  ;
    
    Assert.ifTrue(mode == 0);

    console.writeLine("done")
}

public stackSafeExtensions()
{
    console.write("stackSafeExtensions..");
    
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
    
    console.writeLine("done")
}
A;

//class NilDispatcher
//{
//    int value := 0;
//    
//    testMe(int arg, A a)
//    {
//        value := arg + 1
//    }
//    
//    testMe(int arg, object a)
//    {
//        value := arg
//    }
//    
//    validate(int arg)
//    {
//        Assert.ifTrue(value == arg);
//        
//        value := 0
//    }
//}
//
//nilDispatcherTest()
//{
//    console.write("nilDispatcherTest..");
//    
//    auto d := new NilDispatcher();
//    var d2 := d;
//    A a := nil;
//    var b := nil;
//    
//    d.testMe(1, a) \ back:nil;
//    d.validate(2);
//    
//    d.testMe(1, b) \ back:nil;
//    d.validate(1);
//    
//    d.testMe(1, nil) \ back:nil;
//    d.validate(1);
//
//    d2.testMe(1, a) \ back:nil;
//    d2.validate(1);
//    
//    d2.testMe(1, b) \ back:nil;
//    d2.validate(1);
//    
//    d2.testMe(1, nil) \ back:nil;
//    d2.validate(1);
//
//    console.writeLine("done")
//}

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
    console.write("templateExtensionTest..");
    
    auto a := new A();
    Assert.testExtension(a);
    
    console.writeLine("done")    
}  

// --- ArrayTest ---

arrayTest()
{
    console.write("arrayTest..");
    
    var a1 := new object[]{1,2,3};
    Assert.ifTrue(a1[1] == 2);

    Array a4 := Array.allocate(3);
    a4[0] := 2;
    a4[1] := 1;
    a4[2] := 3;

    Assert.ifTrue(a4[0] == 2);
                            
    Assert.ifFailed({ "abc"[-1] });
    Assert.ifFailed({ "abc"[4] });

    console.writeLine("done")    
}

public ifExpressionTest()
{
    console.write("ifExpressionTest..");
    
    auto c := DynamicSingleton.new(
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

    console.writeLine("done")    
}

public loopExpressionTest()
{
    console.write("loopExpressionTest..");
    
    auto c := DynamicSingleton.new(
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

    console.writeLine("done")    
}
