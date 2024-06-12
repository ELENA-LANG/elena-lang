import extensions;

class A
{
   Func<int,real,object> Handler : event;

   constructor()
   {
      self.Handler := (int n, real m) { console.printLine(n,"+",m,"=",n+m); };
      self.Handler := (int n, real m) { console.printLine(n,"-",m,"=",n-m); };
   }

   invoke(n,m)
   {
      Handler?.(n,m);
   }
}

public program()
{
   var a := new A();
   a.invoke(2,3.2);
}
