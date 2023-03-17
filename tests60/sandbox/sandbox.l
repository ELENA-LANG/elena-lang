import extensions;

public sealed XArray
{
   __string object[] array;

   constructor allocate(int len)
      = new object[](len);

   object[] Value = array;
}

public program()
{
   var a := XArray.allocate(3);

   var o := a.Value;
}