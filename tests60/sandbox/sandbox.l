import extensions;

public program()
{
    Assert.ifTrue("23".toReal() == 23.0);
    console.write(".");
    Assert.ifTrue("123.456789".toReal() == 123.456789);
    console.write(".");
    Assert.ifTrue("-123.456789".toReal() == -123.456789);
    console.write(".");
}