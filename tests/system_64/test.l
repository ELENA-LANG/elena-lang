singleton Helper
{
    test()
    {
    }
}

public program()
{
    var o := Helper;
    
    //Helper
    o.test()
}

startUp = program();
