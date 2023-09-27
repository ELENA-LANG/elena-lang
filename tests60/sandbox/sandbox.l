const int Size = 1000000;

public sealed const struct WindowHandle
{
   embeddable __ptr _handle;

   constructor()
   {
      pointer ptr := UnsafePointer.Empty;

      _handle := ptr
   }
}

public program()
{
   auto h := new WindowHandle();

   extern USER32.CreateWindowExW(h);
}
