import extensions;

public program()
{
    console.printLine("Try");
    try
    {
        var o := new Object();
        o.fail();
    }
    finally
    {
        console.printLine("Finally");
    }
}