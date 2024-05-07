import system'text;

namespace io
{
   public interface BinaryWriter
   {
      get abstract object Source();
    
      abstract close();
        
      abstract bool write(byte[] array, int length);
    
      bool write(byte int)
      {
         byte buffer[4];
            
         buffer[0] := int;
                    
         ^ self.write(buffer, 1)
      }
    
      bool write(short n)
      {
         byte buffer[4];
            
         buffer[0] := n.Low;
         buffer[1] := n.High;
                    
         ^ self.write(buffer, 2)
      }
    
      bool write(char ch)
      {
         int value := ch;

         ^ self.write(value);
      }
    
      bool write(int int)
      {
         byte buffer[4];

         PrimitiveLongOperations.saveToByteArray(int, buffer, 0);
                    
         ^ self.write(buffer, 4)
      }
    
      bool write(long long)
      {
         byte buffer[8];

         PrimitiveLongOperations.saveToByteArray(long, buffer, 0);
                    
         ^ self.write(buffer, 8)
      }
    
      bool write(real r)
      {
         byte buffer[8];
            
         PrimitiveRealOperations.saveToByteArray(r, buffer, 0);
                    
         ^ self.write(buffer, 8)
      }

      bool write(string s)
      {
         byte buffer[256];
         int  len := s.Length;
         int  buf_len := 256;
         int  s_len := 0;
         int  index := 0;
         while (len > 0)
         {
            if (len < 256)
            { 
               buf_len := len;
               s_len := len;
            }
            else    
            { 
               buf_len := 256; 
               s_len := 256;
            };
               
            UTF8Encoder.toByteArray(s, index, ref s_len, buffer, 0, ref buf_len);
               
            write(buffer, buf_len);
               
            len := len - s_len;
            index := index + s_len
         };

         ^ true
      }

   }
    
   public sealed BinaryStreamWriter : BinaryWriter
   {
      Stream _stream;

      constructor(Stream stream)
      {
         _stream := stream
      }

      object Source = _stream;

      close() => _stream;

      bool write(byte[] array, int length)
      {
         _stream.write(array, length);

         ^ true
      }
   }
    
   // --- TextWriter ---
    
   public interface TextWriter
   {
      TextBuilder _buffer;
      string      _newLineConstant;
      int         _newLineLength;
    
      get abstract object Source();
    
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

   // --- StreamWriter ---   
   public class StreamWriter : TextWriter
   {
      Stream  _stream;
      Encoder _encoder;
    
      constructor new(Stream stream, Encoder encoder)
      {
         _buffer := new TextBuilder();
         _stream := stream;
         _encoder := encoder;
    
         _newLineConstant := newLineConstant;                
         _newLineLength := _newLineConstant.Length;
      }
    
      constructor new(path)
         <= new(FileStream.openForReWrite(path), UTF8Encoder);
            
      constructor new(path, Encoder encoder)
         <= new(FileStream.openForReWrite(path), encoder);
            
      object Source = _stream;        
            
      Encoder Encoder = _encoder;
            
      write(char[] input, int length)
      {
         int  total := length;
         byte buffer[128];
         int  blockLength := 0;
         int  bufferLength := 0;
         int  offset := 0;
            
         while (total > 0)
         {
            if(total < 32)
            { 
               blockLength := total 
            }
            else
            { 
               blockLength := 32
            };
                    
            _encoder.convertTo(input, offset, ref blockLength, buffer, 0, ref bufferLength);
                
            _stream.write(buffer, bufferLength);
                
            total := total - blockLength;
            offset := offset + blockLength
         }
      }
        
      close()
      {
         _stream.close()
      }
   }
}