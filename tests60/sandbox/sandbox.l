import extensions;
//import system'math;
//
//public extension lazyOp
//{
//   whileFalse(Func action)
//   {
//      until(self)
//      {
//         action()
//      }
//   }        
//}
//
//public program()
//{
//    var n := 1;
//    
//    (:lazy (n.sqr().mod(100) == 64)).whileFalse({ n := n + 1 });
//    
//    console.printLine(n)
//}

//#new __symbol aliases;

//#let aliases["myint"]  := system'IntNumber;

//#load sandbox'aliases;

//myint X = 2;
/*
class MyException : Exception
{
    constructor new(s)
        <= super new(s);
}
*/
class MyException2 : Exception
{
    constructor new(s)
        <= super new(s);
}

singleton ExecTester
{
/*    break()
    {
        MyException.new("OnError handler works").raise()
    }
*/    
    break2()
    {
        MyException2.new("OnError2 handler works").raise()
    }
/*    
    break3()
    {
        MyException2.new("Nested OnError2 handler works").raise()
    }*/
}

public program()
{
//    bool passed := false;
    try
    {
        ExecTester.break2()
    }
    catch::
    {
//        function(MyException e)
//        {
            //Assert.ifTrue(false)
//        }
        function(MyException2 e)
        {
//            passed := true;
        }
    };
//    Assert.ifTrue(passed);
    console.write(".");
/*    
    bool passed2 := false;
    try
    {
        try
        {
            MyException2.new("Nested OnError2 handler works").raise()
        }
        catch(MyException e)
        {
            console.writeLine("!!Wrong handler");
            Assert.ifTrue(false)
        }
    }
    catch(MyException2 e)
    {
        passed2 := true;
    };
    Assert.ifTrue(passed2);
    
    console.write(".")*/
}
