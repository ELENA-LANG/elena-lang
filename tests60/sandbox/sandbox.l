/*import extensions;

extension op : A
{
   printMe()
   {
      Console.printLine(self, " was called")
   }   
}

class A
{
   constructor() {}
}

public Program()
{
   var o := op;
   
   auto a := new A();
   
   a.printMe();
}
*/

    import extensions;
    
    class A
    {
       extension printMe<T>(T arg)
       {
          Console.printLine(arg, " was called from ", self);

          ^ arg
       }
    
       constructor() {}
    }
        
    public Program()
    {
       auto a := new A();
       
       a.printMe(2);
       Assert.ifFailed({ 2.printMe(2) });
    }
