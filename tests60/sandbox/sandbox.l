import extensions;

public program()
{
    Assert.ifTrue(-2l.toInt() == -2);
    console.write(".");

    Assert.ifTrue(-2.toLong() == -2l);
    console.write(".");
    
    Assert.ifTrue(-2.0.toInt() == -2);
    console.write(".");

    Assert.ifTrue(-2.toReal() == -2.0);
    console.write(".");
        
    Assert.ifTrue(-2.0.toLong() == -2l);
    console.write(".");

    Assert.ifTrue(-2l.toReal() == -2.0);
    console.write(".");
}