import system'text;
import system'collections;

namespace io
{
   public sealed class File
   {
      string _path;

      constructor assign(string path)
      {
         _path := path
      }

      string cast() = _path;
    
      wide cast() = _path;

      bool Available
         = fileControl.isAvailable(_path);

      TextReader textreader() = fileControl.newReader(_path);

      TextWriter textwriter() = fileControl.writer(_path);

      TextWriter logger() = fileControl.logger(_path);

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
         File.saveContent(_path, content);
      }    

      string readContent()
         = File.readContent(_path);
        
      wide readWideContent()
         = File.readWideContent(_path);

      delete()
         = fileControl.delete(_path);

      static saveContent(string path, string content)
      {
         using(TextWriter writer := fileControl.writer(path)) {
            writer.write(content);
         }         
      }

      static saveContent(string path, string content, Encoder encoder)
      {
         using(TextWriter writer := fileControl.writer(path, encoder)) {
            writer.write(content);
         }         
      }

      static readContentTo(string path, TextBuilder output)
      {
         using(TextReader reader := fileControl.newReader(path)) {
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

      static string readContent(string path)
      {
         auto output := new TextBuilder();
            
         File.readContentTo(path, output);
            
         ^ output.Value
      }

      static wide readWideContent(string path)
      {
         auto output := new TextBuilder();
            
         File.readContentTo(path, output);
            
         ^ output.Value
      }
      
      static TextWriter textwriter(string path) 
         = fileControl.newWriter(path);         
      
      static TextWriter textreader(string path) 
         = fileControl.newReader(path);         
      
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

      string[] getFiles()
         <= getFiles("*.*");
        
      string[] getFiles(string mask)
         = Directory.getFiles(_path, mask);

      create()
      {
         directoryControl.create(_path);
      }     

      delete()
         = directoryControl.delete(_path);

      static string[] getFiles(string path)
         = getFiles(path, "*.*");

      static string[] getFiles(string path, string mask)
      {
         auto l := new List<string>();
         SearchFiles(path, mask, l);
            
         ^l.Value;         
      }
   }   
}