import extensions;

public Program()
{
   int n := 2;
   int m := 3;
   int k := 4;
   int l := 3;
                    
   n := IntNumber.MinValue;
   string s := n.toPrintable();
   Console.writeLine(s);
   Assert.ifTrue(s == "-2147483648");
   Console.write(".");
}