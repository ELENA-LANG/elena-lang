import system'text;

namespace io
{
   const int F_OK     = 0;

   const int SEEK_SET = 0;
   const int SEEK_CUR = 1;
   const int SEEK_END = 2;

   // --- FileStream ---
   public struct FileStream : Stream
   {
      handle _handle;
        
      constructor openForRead(path)
         <= new(path, "r+");
        
      constructor openForReWrite(path)
         <= new(path, "w+");
    
      constructor openForAppend(path)
         <= new(path, "a");

      constructor openForEdit(path)
            <= new(path, "w");
        
      constructor new(string path, string attributes)
      {
         _handle := extern libc.fopen
                          (path,
                          attributes);
                          
         if(-1 == _handle)
                { IOException.new("Cannot open the file").raise() }
      }
        
      write(byte[] dump, int length)
      {
         extern libc.fwrite(
            dump,
            1,
            length,
            _handle)
      }
    
      int read(byte[] dump, int length)
      {
         int n := extern libc.fread(
                     dump,
                     1,
                     length,
                     _handle);
    
         ^ n
      }
    
      int Index
      {    
         get()
         {
            int ret := extern libc.lseek(
                        _handle,
                        0,
                        SEEK_CUR);
                    
            ^ ret
         } 
            
         set(index)
         {
            int ret := extern libc.lseek(
                        _handle,
                        index,
                        SEEK_SET);
         }
      }
        
      int Length
      {
         get() 
         { 
            int ret := extern libc.lseek(
                        _handle,
                        0,
                        SEEK_END);
                    
                ^ ret

         }
      }

      indexed internal setLength(int length)
      {
         NotSupportedException.raise()
      }

      close()
      {
         extern libc.fclose(_handle)
      }
   }

   singleton fileControl
   {
      TextReader newReader(path) = TextStreamReader.new(path);

      TextWriter writer(path) = StreamWriter.new(FileStream.openForReWrite(path), UTF8Encoder);

      TextWriter writer(string path, Encoder encoder) = StreamWriter.new(FileStream.openForReWrite(path), encoder);

      TextWriter logger(path) = StreamWriter.new(FileStream.openForAppend(path), UTF8Encoder);

      bool isAvailable(path)
      {
         int ret := extern libc.access(path as:String, F_OK);
            
         ^ ret == 0
      }      

      bool delete(path)
      {
         int ret := extern libc.remove(path as::String);
            
         ^ ret == 0
      }
   }

   singleton directoryControl
   {
      bool isAvailable(path)
      {
         string s := path as::String;
         if:not(s.endingWith("/"))
            s := s + "/";

         int ret := extern libc.access(s, F_OK);
            
         ^ ret == 0
      }      
   }
}