import system'routines;
import extensions;

public program()
{
   var t := (1,"abc");

   console.printLine("Item1=",t.Item1,", Item2=", t.Item2);

   var a;
   var b;

   a,b := t;
   console.printLine("a=",a,", b=", t.Item2);
}