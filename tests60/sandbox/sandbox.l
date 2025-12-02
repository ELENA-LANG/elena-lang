import extensions;

public const struct ExtensionDispatcher
{
   embeddable __string pointer[] _array;

   protected constructor allocate(int length)
      = new pointer[](length);
        
   internal constructor copy(pointer[] array, int length)
      <= allocate(length)
   {
      for(int i := 0; i < length; i += 1)
      {
         _array[i] := array[i]
      }
   }
}

/*class B;

class A
{
   readonly B b;

   constructor new(B b)
   {
      this b := b
   }

   testReadOnly(B arg)
   {
      b := arg // !! raises an error : 
   }
   
}*/

public program()
{
//   A a := A.new(new B());
//   a.testReadOnly(new B());
}
