//import extensions;
//import xml;
//import system'io;
//import system'text;
//import extensions'text;

public program()
{
   X x := new X();
   while (x.isAvailable)
   {
      x.next()
   };
   
   var s := "abc";
   for(int i := 0;i < s.Length; i++) {
      Console.writeLine(s[i])
   };
}

X
{
   x;
   
   constructor() { x := 0 }
   
   bool isAvailable
      = x < 3;
   
   next()
   {
      x := x + 1;
   }
}
