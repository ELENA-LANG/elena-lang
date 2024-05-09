import extensions;
import system'collections;

public program()
{
   var l := 1;
   var r := 1;

//   var f := (object l,object r => (l < r) ? -1 : (l == r ? 0 : 1));

  int t := (l < r) ? -1 : (l == r ? 0 : 1);

//   int t := l == r ? 0 : 1;

   console.writeLine(t)
}