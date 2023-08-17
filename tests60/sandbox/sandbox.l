import extensions;

public program()
{
    console.write("conversionTests:");

    var n := "23".toInt();
    var sn := n.toString();
    Assert.ifTrue(sn == "23");
    console.write(".");

    Assert.ifTrue("1234567890123456".toLong() == 1234567890123456l);
    Assert.ifTrue("-123".toInt() == -123);
    Assert.ifTrue("-1234567890123456".toLong() == -1234567890123456l);
    console.write(".");

    Assert.ifTrue("23".toReal() == 23.0);
    Assert.ifTrue("123.456789".toReal() == 123.456789);
    Assert.ifTrue("-123.456789".toReal() == -123.456789);
    console.write(".");

    Assert.ifTrue(-2l.toInt() == -2);
    Assert.ifTrue(-2.toLong() == -2l);
    
    Assert.ifTrue(-2.0.toInt() == -2);
    Assert.ifTrue(-2.toReal() == -2.0);
        
    Assert.ifTrue(-2.0.toLong() == -2l);
    Assert.ifTrue(-2l.toReal() == -2.0);
    console.write(".");

    int n2 := 1234;
    short w := n2;
    Assert.ifTrue(w == 1234);
    console.write(".");
    string s := n2.toString();
    Assert.ifTrue(s == "1234");
    console.write(".");
    int n3 := s.toInt();
    Assert.ifTrue(n3 == n2);
    console.writeLine(".");
}