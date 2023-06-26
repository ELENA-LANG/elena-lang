import extensions;

singleton Tester
{
   digit(o)
   {
      console.printLine(o);
   }
}
   
public program()
{
   var m := mssg digit;

   m(Tester, 1);
}
