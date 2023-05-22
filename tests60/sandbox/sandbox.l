import extensions;

A
{
    generic(n)
    {
        // __received refers to the current message
        console.printLine("message ",__received," is invoked")
    }
    
    generic(n1,n2)
    {
        console.printLine("message ",__received," is invoked")
    }
}

public program()
{
    var a := new A();
    a.testMe(2);
    a.andMeTo("abc");
    a.testWithTwoArguments(2,"abc");
}