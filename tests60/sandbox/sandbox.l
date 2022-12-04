import extensions;

public class XVariable : BaseVariable, Reference<object> //,
//   info<"A generic variable.<br/>Extends an assigned value">
{
   constructor(value)
//         : info<"Creates the variable">
   {
//      this value := value
   }

   append(val)
   {
//      if (value == nil) {
         value := val
//      }
//      else {
//         value += val;
//      }
   }
}

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

public program()
{
   var c := 2;

    var v := new XVariable(2);
    v.append(3);

    console.writeLine(v);

//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}


