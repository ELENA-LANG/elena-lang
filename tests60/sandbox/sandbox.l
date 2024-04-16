import extensions;

tester(params int[] others)
{
   console.writeLine("!!")
}

public variadicTest()
{
    // run-time
    var f := tester;
    f(1,3,3);
/*    tester.test2("hello",3,3);
    
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
    
    console.write(".")*/
}

public program()
{
   variadicTest()
}