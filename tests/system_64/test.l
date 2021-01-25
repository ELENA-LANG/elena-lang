public program()
{
    console.writeLine("--");
    
    try
    {
        var o := 2;
        o.test()
    }
    catch(Exception e)
    {
        console.writeLine(e.toPrintable());
    }
    
//    console.writeLine("Hello World - Привет Мир");
}
