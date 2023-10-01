import extensions;

sealed A
{
   Func1 onFire : event;

   constructor new()
   {
      onFire := (arg){ self.test(arg) }
   }

   private test(arg)
   {
      console.printLine(arg)
   }

   fire(arg)
   {
      onFire(arg)
   }
}
 
public program()
{
   A a := A.new();

   a.fire("Hello");
}
