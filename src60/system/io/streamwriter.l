import system'text;

namespace io
{
   public interface BinaryWriter
   {
      Stream _stream;
        
      Stream Stream = _stream;
    
      close() => _stream;
        
      write(byte[] array, int length)
      {
         _stream.write(array, length)
      }
    
      write(byte int)
      {
         byte buffer[4];
            
         buffer[0] := int;
                    
         self.write(buffer, 1)
      }
    
      write(short n)
      {
         byte buffer[4];
            
         buffer[0] := n.Low;
         buffer[1] := n.High;
                    
         self.write(buffer, 2)
      }
    
      write(char ch)
      {
         int value := ch;

         self.write(value);
      }
    
      write(int int)
      {
         byte buffer[4];

         PrimitiveLongOperations.saveToByteArray(int, buffer, 0);
                    
         self.write(buffer, 4)
      }
    
      write(long long)
      {
         byte buffer[8];

         PrimitiveLongOperations.saveToByteArray(long, buffer, 0);
                    
         self.write(buffer, 8)
      }
    
      write(real r)
      {
         byte buffer[8];
            
         PrimitiveRealOperations.saveToByteArray(r, buffer, 0);
                    
         self.write(buffer, 8)
      }
   }
    
   public sealed BinaryStreamWriter : BinaryWriter
   {
      constructor(Stream stream)
      {
         _stream := stream
      }
   }
    
   // --- TextWriter ---
    
   public interface TextWriter
   {
      TextBuilder _buffer;
      string      _newLineConstant;
      int         _newLineLength;
    
      get abstract Stream Stream();
    
      get abstract Encoder Encoder();
    
      abstract close();
        
      abstract write(char[] array, int length);
        
      writeLine(line)
      {
         self.write(line).write(_newLineConstant)
      }
        
      write(object)
         <= write(object.Printable);
        
      write(string line)
      {
         _buffer.clear().write(line);
            
         int length := _buffer.Length;
            
         self.write(cast char[](_buffer), length)
      }
        
      write(wide line)
      {
         _buffer.clear().write(line);
            
         int length := _buffer.Length;
            
         self.write(cast char[](_buffer), length)
      }
        
      writeLine()
         <= write(_newLineConstant);
   }
}