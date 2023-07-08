import extensions;

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

public program()
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


//    var buffer := new system'text'TextBuilder();
//    
//    buffer.write:"a";
//    buffer.write:"b";
//    buffer.write:"c";
//
//    var s := buffer.Value;

}
