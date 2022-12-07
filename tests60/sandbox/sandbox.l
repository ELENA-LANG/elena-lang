import extensions;

function(acc)
    = (n => acc.append:n);

public program()
{
//   var x := function(new Variable(2));
//   x(1);

   var n := new Variable(2);
   n.append(3);

   console.writeLine(n); 
}
