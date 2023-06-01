import extensions;

public class A
{
   constructor new()
   {
      console.printLine("Calling a constructor");
   }
}

public program()
{
   var c := new ClassReference("sandbox'A");
   var a := c.new();
}