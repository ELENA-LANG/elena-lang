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
    
      read(byte[] dump, ref int retVal)
      {
         int n := extern libc.fread(
                     dump,
                     1,
                     retVal,
                     _handle);
    
         retVal := n
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

      close()
      {
         extern libc.fclose(_handle)
      }
   }

   // --- StreamReader ---
   public class TextStreamReader : TextReader
   {
      Stream  _stream;
      Encoder _encoder;
      bool    _open;
    
      constructor new(Stream stream, Encoder encoder)
      {
         _output := new TextBuilder();
         _newLineConstant := newLineConstant;                
         _newLineLength := _newLineConstant.Length;
            
         _stream := stream;
         _encoder := encoder;
         _open := true;
      }
    
      constructor new(path)
         <= new(FileStream.openForRead(path), UTF8Encoder);
            
      constructor new(string path, Encoder encoder)
         <= new(FileStream.openForRead:path, encoder);
    
      bool Available = _open;
    
      Stream Stream = _stream;
    
      Encoder Encoder = _encoder;
    
      read(char[] output, ref int length)
      {
         int  bufferLength := 64;
         byte buffer[64];
         _stream.read(buffer, ref bufferLength);
         if(bufferLength == 0)
         {
            _open := false; 
            length := 0;
         }
         else
         {
            int read := bufferLength;
    
            _encoder.convertFrom(buffer, 0, ref bufferLength, output, 0, ref length);
                
            if(read > bufferLength)
               { _stream.reduceIndex(read - bufferLength) }
         }           
      }
        
      close()
      {
         _stream.close()
      }
   }

   singleton fileControl
   {
      TextReader newReader(path) = TextStreamReader.new:path;
   }
}