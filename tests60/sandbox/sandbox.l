import extensions;

public program()
{
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