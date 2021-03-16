import extensions;

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
//    console.write("variadicTest..");
    
    // run-time
    var tester := new VariadicTested();
    tester.test1(1,3,3);
//    tester.test2("hello",3,3);
//    
//    tester.validate();
//    
//    // compile-time
//    auto tester2 := new VariadicTested();
//    tester2.test1(1,3,3);
//    tester2.test2("hello",3,3);
//    
//    tester2.validate();
//    
//    console.writeLine("done")
}

public program()
{
    variadicTest()
}