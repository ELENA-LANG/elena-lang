import system'routines;
import extensions;

singleton Tester
{
   int,string get()
   {
      ^ (1,"abc")
   }
}

public program()
{
//   var t := Tester.get();
//   console.printLine("t.Item1=",t.Item1, ",t.Item2=", t.Item2);

   var a, b := Tester.get();
   console.printLine("t.Item1 aka a=",a, ",t.Item2 aka b=", b);
}