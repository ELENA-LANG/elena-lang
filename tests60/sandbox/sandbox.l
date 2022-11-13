import extensions;

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

A
{
   s;

   cast w(string s)
   {
      this s := s;
   }

   string toPrintable()
      = "A:" + s;
}

public program()
{
   var o := 2w;
   console.writeLine(o);

//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}

