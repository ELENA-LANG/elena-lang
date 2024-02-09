import extensions;

public program()
{
    console.write("controlTest:");
    
    var b1 := false;
    console.write(".");
    if (true)
    {
        b1 := true
    };
    console.write(".");
    
    var b2 := nil;
    console.write(".");
    if (false)
    {
        b2 := false;
    }
    else
    {
        b2 := true
    };
    console.write(".");
    
    Assert.ifTrue(b1);    
    console.write(".");
    Assert.ifTrue(b2);    
    console.write(".");
}