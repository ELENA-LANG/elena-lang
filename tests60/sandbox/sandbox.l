import extensions;

abstract class Base
{
   protected constructor new()
   {
   }   
}

abstract class A : Base
{
   protected constructor new()
      <= super new();
}
 
B : A
{
   constructor new()
      <= super new()
   {
   }
}

public program()
{
   var b := B.new();
}