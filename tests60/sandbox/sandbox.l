import extensions;

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

conversionTests()
{
    console.write("conversionTests");
    int n := 1234;
    string s := n.toString();
    Assert.ifTrue(s == "1234");
    console.writeLine(".");
}

public program()
{
   console.writeLine("Hello");

//   conversionTests()

//   string s := "abc";

//   var ch := s[0];

//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}

