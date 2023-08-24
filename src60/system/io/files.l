namespace io
{
   public sealed class File
   {
      string _path;

      constructor assign(string path)
      {
         _path := path
      }

      bool Available
         = fileControl.isAvailable(_path);

      TextReader textreader() = fileControl.newReader(_path);
   }

   // --- Directory ---
    
   public sealed class Directory
   {
      string _path;

      constructor assign(string path)
      {
         _path := path
      }

      bool Available
         = directoryControl.isAvailable(_path);    
   }   
}