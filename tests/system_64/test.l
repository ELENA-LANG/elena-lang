A;

singleton Helper
{
    test()
    {
        var a := new A();
    }
}

public program()
{
    var o := Helper;
    
    //Helper
    o.test()
}

startUp = program();
