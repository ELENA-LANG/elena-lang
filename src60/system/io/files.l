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

      TextWriter textwriter() = fileControl.writer(_path);

      saveContent(string content)
      {
         using(TextWriter writer := fileControl.writer(_path)) {
            writer.write(content);
         }
      }    

      delete()
         = fileControl.delete(_path);

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

      create()
      {
         directoryControl.create(_path);
      }     

      delete()
         = directoryControl.delete(_path);      
   }   
}