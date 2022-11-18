import extensions;

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

B
{
   convert(string s)
   {
      console.writeLine("<string>");
   }

//  convert(o)
//   {
//      console.writeLine("general");
//   }
}

A : B
{
   convert(o)
   {
      console.writeLine("general");
   }

   convert(int n)
   {
      console.writeLine("<int>");
   }

}

public program()
{
   var a := new A();
   a.convert(2);
   a.convert("abc");
   a.convert(self);

//   byte ch := 45;
//   byte ch2 := 32;
//   byte tmp[4];
//   tmp[0] := ch;
//   tmp[1] := ch2;
  
//   int val := intConvertor.convert($50);
              
//   char c := $50;

//   console.writeLine(c)

//   string s := "abc";

//   var ch := s[0];

//   console.writeLine(ch);

//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}

