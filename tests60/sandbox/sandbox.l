import extensions;

public Program()
{
/*   long n := 20;
   int r := 3;

   long rest := n.mod(r);
   n := n / r;

   Console.writeLine(rest);
   Console.writeLine(n);

   long m := -6;
   m := 0 - m;
   Console.writeLine(m);

*/

   int n := 2;
   int m := 3;
   int k := 4;
   int l := 3;
                    
   n := /*IntNumber.MinValue*//*-2147483647*/-23;
   string s := n.toPrintable();
   Console.writeLine(s);
   Assert.ifTrue(s == "-2147483648");
   Console.write("."); 
}