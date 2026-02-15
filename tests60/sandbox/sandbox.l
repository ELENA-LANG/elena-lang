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

   //string s := convert(-214748364, 10);
   int r := 10;
   long n := /*-*//*2*/147483648l;

   long l := r;
//   Console.writeLine(l); 

   //bool b := n < r;
   bool b := n < l;
   Console.writeLine(b); 

//   bool b := n >= r;
//   Console.writeLine(b); 

//   bool b2 := n >= l;
//   Console.writeLine(b2); 


//      while (n >= r) {
//         long rest := n.mod(r);

//         n := n / r;

//   Console.writeLine(n.toString()); 
//   Console.writeLine(rest.toString()); 


//         tmp := rest;
//         tmp := tmp + 48;
//         buffer[index] := tmp;
//         index := index - 1;
//         len := len + 1;
//      };



//   long m := n / r;   
//   long rest := n.mod(r);

   string s := n.toString();
   Console.writeLine(s); 
//   Console.writeLine(m.toString()); 
//   Console.writeLine(rest.toString()); 

//   int n := 2;
//   int m := 3;
//   int k := 4;
//   int l := 3;
                    
//   n := /*IntNumber.MinValue*//*-2147483647*/-214748/*364*/;
//   string s := n.toPrintable();
//   Console.writeLine(s);
//   Assert.ifTrue(s == "-2147483648");
//   Console.write("."); 
}