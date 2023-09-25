import system'text;

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

      get int Length()
      {
         int    fileLen := 0;
         using(Stream stream := FileStream.openForRead(_path)) {
            fileLen := stream.Length;        
         };
            
         ^ fileLen
      }

      saveContent(string content)
      {
         using(TextWriter writer := fileControl.writer(_path)) {
            writer.write(content);
         }
      }    

      private read(TextBuilder output)
      {
         using(TextReader reader := fileControl.newReader(_path)) {
            char       buffer[128];
            int        read := 0;
            while (reader.Available)
            {
               read := 128;
               reader.read(buffer, ref read);
                   
               output.write(buffer, read)
            };            
         }
      }

      string readContent()
      {
         auto output := new TextBuilder();
            
         self.read(output);
            
         ^ output.Value
      }
        
      wide readWideContent()
      {
         auto output := new TextBuilder();
            
         self.read(output);
            
         ^ output.Value
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