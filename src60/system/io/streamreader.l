import system'text;

namespace io
{
   // --- BinaryReader ---
    
   public interface BinaryReader
   {      
      abstract int read(byte[] array, int length);
    
      get abstract object Source();
    
      abstract close();
    
      get abstract bool Available();
                
      readExact(byte[] array, int length)
      {
         int read := self.read(array, length);
             
         if(length > read)
            { IOException.new("Cannot read the specified amount").raise() }
      }
    
      read(ref byte int)
      {
         byte buffer[4];
         self.readExact(buffer, 1);
         
         byte b := buffer[0];
                              
         int := b
      }
    
      read(ref short int)
      {
         byte buffer[4];
            
         self.readExact(buffer, 2);
                
         PrimitiveIntOperations.loadFromByteArray(buffer, 0, ref int retVal := 0);

         short tmp := retVal;
         int := tmp
      }
    
      read(ref char ch)
      {
         byte buffer[4];
            
         self.readExact(buffer, 4);
                
         PrimitiveIntOperations.loadFromByteArray(buffer, 0, ref int retVal := 0);

         char tmp := retVal;
         ch := tmp
      }
    
      read(ref int int)
      {
         byte buffer[4];
            
         self.readExact(buffer, 4);

         PrimitiveIntOperations.loadFromByteArray(buffer, 0, ref int retVal := 0);

         int := retVal                
      }
    
      read(ref long long)
      {
         byte buffer[8];
            
         self.readExact(buffer, 8);
                
         PrimitiveLongOperations.loadFromByteArray(buffer, 0, ref long retVal := 0l);

         long := retVal
      }
    
      read(ref real real)
      {
         byte buffer[8];
            
         self.readExact(buffer, 8);
                
         PrimitiveRealOperations.loadFromByteArray(buffer, 0, ref real retVal := 0.0);

         real := retVal
      }
   }
    
   public sealed BinaryStreamReader : BinaryReader
   {
      Stream _stream;
      
      constructor(Stream stream)
      {
         _stream := stream
      }

      object Source = _stream;

      close() => _stream;

      get bool Available()
      {
         int position := _stream.Index;
            
         ^ position < _stream.Length
      }

      int read(byte[] array, int actualLength)
      {
         ^ _stream.read(array, actualLength)
      }
   }

   // --- TextReader ---
   public interface TextReader
   {
      abstract int read(char[] array, int length);
    
      get abstract object Source();
    
      get abstract Encoder Encoder();
    
      abstract close();
        
      get abstract bool Available();

      abstract string readLine();

      abstract wide readWideLine();
   }
    
   // --- TextReader ---
   public abstract BaseTextReader : TextReader
   {
      TextBuilder _output;
      string      _newLineConstant;
      int         _newLineLength;
    
      string readLine()
      {
         string line := emptyString;
            
         int len := -1;
         while (len == -1)
         {
            len := _output.indexOf(0, _newLineConstant);
                
            if(len == -1)
            {
               char buffer[64];
               int  bufferLen := 64;
                    
               self.read(buffer, ref bufferLen);
               if(bufferLen == 0)
               { 
                  len := -2 // !! to indicate the eof 
               }
               else
               {
                  _output.write(buffer, bufferLen)
               }
            }
         };
            
         if(len < 0) 
         {
            line := cast string(_output);
            _output.clear();
         }
         else
         {
            line := _output.Substring(0, len);
            _output.delete(0, len + _newLineLength)
         };
    
         ^ line
      }
        
      wide readWideLine()
      {
         wide line := emptyWideString;
         wide nl := cast wide(_newLineConstant);
         int  nl_len := nl.Length;
            
         int len := -1;
         while (len == -1)
         {
            len := _output.indexOf(0, nl);
                
            if(len == -1)
            {
               char buffer[64];
               int  bufferLen := 64;

               self.read(buffer, ref bufferLen);
               if(bufferLen == 0)
               { 
                  len := -2 // !! to indicate the eof 
               }
               else
               {
                  _output.write(buffer, bufferLen)
               }                                          
            }
         };

         if(len < 0) 
         {
            line := cast wide(_output);
            _output.clear();
         }
         else
         {
            line := _output.WideSubstring(0, len);
            _output.delete(0, len + nl_len)
         };
    
         ^ line
      }
   }

   // --- StreamReader ---
   public class TextStreamReader : BaseTextReader
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
         <= new(FileStream.openForRead(path), encoder);
    
      bool Available = _open;
    
      object Source = _stream;
    
      Encoder Encoder = _encoder;
    
      int read(char[] output, int length)
      {
         int  bufferLength := 64;
         byte buffer[64];
         int actualLen := _stream.read(buffer, bufferLength);
         if(actualLen == 0)
         {
            _open := false; 
         }
         else
         {
            int read := actualLen;
    
            actualLen := _encoder.convertFrom(buffer, 0, ref bufferLength, output, 0, length);
                
            if(read > bufferLength)
               { _stream.reduceIndex(read - bufferLength) }
         };

         ^ actualLen
      }
        
      close()
      {
         _stream.close()
      }
   }
}