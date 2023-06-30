import extensions;

A
{
   test(int n) {}
   test(long l) {}
}

public program()
{
    int n := 1234;
    string s := n.toString();
    int n2 := s.toInt();

//   int n := "abc".Length;
//   console.writeLine(n);
}
