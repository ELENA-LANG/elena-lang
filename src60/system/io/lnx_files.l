import system'text;

namespace io
{
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
   }

   singleton directoryControl
   {
   }
}