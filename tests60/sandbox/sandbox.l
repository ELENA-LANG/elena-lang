import extensions;

function(acc)
    = (n => acc.append:n);

public program()
{
   var x := function(new Variable(2));
   var n := x(1);

   console.writeLine(n); 
}
