public sealed const struct XUnsafePointer
{
   embeddable __ptr _pointer;

   get static XUnsafePointer Default()
      = new XUnsafePointer();   

   constructor()
   {
   }
}

public program()
{
   XUnsafePointer ptr := XUnsafePointer.Default;
}