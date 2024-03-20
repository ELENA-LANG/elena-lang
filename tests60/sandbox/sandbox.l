import system'collections;
import extensions;

public program()
{
  var a := new List();

  {
     var a := 2;

     super a.append(a); 
  };

  console.printLine(a.asEnumerable());
}
