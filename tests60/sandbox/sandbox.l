import system'collections;
import extensions;
import system'routines;

public program()
{
   var a := new int[] { 3, 4, 1 };

   var r := a.sort:(p,n => p < n);

   console.printLine(r.asEnumerable())
}