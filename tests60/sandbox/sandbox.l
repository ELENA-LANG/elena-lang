import extensions;

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

singleton Tester
{
   internal test(byte[] buffer)
   {
      console.writeLine(buffer[0]);
      console.writeLine(buffer[1]);
   }
}

//conversionTests()
//{
//    console.write("conversionTests");
//    int n := 1234;
//    string s := n.toString();
//    Assert.ifTrue(s == "1234");
//    console.writeLine(".");
//}

public program()
{
   byte b1 := 10;
   byte b2 := 20;

   byte tmp[3];
   tmp[0] := b1;
   tmp[1] := b2;

   Tester.test(tmp);

//   conversionTests()

//   string s := "abc";

//   var ch := s[0];

//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}

