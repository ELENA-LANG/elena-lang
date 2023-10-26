public sealed const struct XUnsafePointer
{
   embeddable __ptr _pointer;

   constructor()
   {
   }

   get static XUnsafePointer Default()
      = new XUnsafePointer();   
}

public program()
{
   XUnsafePointer ptr := XUnsafePointer.Default;
}