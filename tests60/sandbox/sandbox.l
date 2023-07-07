import extensions;

public program()
{
    wide s := "abc"w;

    Assert.ifTrue(s == "abc"w);
    console.write(".");
    wide s2 := "de"w;
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde"w);
    console.write(".");

    Assert.ifTrue("s1"w < "s2"w);
    Assert.ifFalse("s2"w < "s2"w);
    Assert.ifFalse("s2"w < "s1"w);

    console.writeLine(".");
}
