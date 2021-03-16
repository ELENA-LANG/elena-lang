import extensions;

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

exceptionTest()
{

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

public program()
{
    exceptionTest()
}