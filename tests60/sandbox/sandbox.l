import extensions;

public sealed const struct XHandle
{
   handle _handle;

   constructor()
   {
      _handle := Handle.Default;
   }

   bool equal(handle h)
   {
      ^ _handle == h;
   }
}

public program()
{
   handle h := Handle.Default;
   XHandle xh := new XHandle();
   if (xh.equal(h)) {
      console.writeLine("Works!")
   }
}