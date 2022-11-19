import extensions;

A
{
   int get(int x)
      = 2;

   multiret string get(int x)
      = "str:2";
}

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

public program()
{
   A a := new A();
   int n := a.get(1);
   string s := a.get(1);

   console.writeLine(n);
   console.writeLine(s);

//   byte ch := 45;
//   byte ch2 := 32;
//   byte tmp[4];
//   tmp[0] := ch;
//   tmp[1] := ch2;
  
//   string s := "abc";

//   var ch := s[2];

//   console.writeLine(ch);

//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}

