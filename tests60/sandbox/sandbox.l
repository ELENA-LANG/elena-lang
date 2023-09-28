const int Size = 1000000;

public sealed const struct WindowHandle
{
   handle _handle;

   constructor()
   {
      handle ptr := Handle.Empty;

      _handle := ptr
   }
}

public program()
{
   auto h := new WindowHandle();

   extern USER32.CreateWindowExW(h);
}
