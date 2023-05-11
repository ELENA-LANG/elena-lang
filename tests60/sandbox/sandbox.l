import extensions;

public program()
{
    console.printLine("Try");
    try
    {
        var o := new Object();
        o.fail();
    }
    catch(Exception e)
    {
        console.printLine("Error!");
    }
    finally
    {
        console.printLine("Finally");
    }
}