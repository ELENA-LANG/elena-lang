import extensions;

singleton Tester
{
   printMe(int n)
   {
      console.writeLine(n)
   }
} 
 
public program()
{
   int n := 23;

   Tester.printMe:12;
   Tester.printMe:n;
}  