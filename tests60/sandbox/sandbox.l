import extensions;

function(acc)
    = (n => acc.append:n);

public program()
{
   var v := function(new Variable(2));

   console.writeLine(v(2)); 
}
