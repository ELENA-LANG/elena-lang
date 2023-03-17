import extensions;

public sealed XArray
{
   __string object[] array;
    
   /// Creates the array with the specified length
   constructor allocate(int len)
      = new object[](len);

   /// Sets the specified array member
   setAt(int n, object o)
   {
      array[n] := o
   }

   object[] Value = array;
}

public program()
{
   var a := XArray.allocate(3);
   a[0] := 1;
   a[1] := 2;

   var o := a.Value;
}