import system'routines;
import system'dynamic'expressions;
import extensions;

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
