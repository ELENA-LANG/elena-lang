import system'io;

namespace io
{
   public sealed ByteArrayWriter : BinaryWriter
   {
      byte[]         _buffer;
      int            _start;
      int            _end;
      Reference<int> _position;

      constructor new(byte[] buffer, int start, int length)
      {
         _buffer := buffer;
         _start := start;
         _end := start + length;
         _position := start;
      }

      constructor new(byte[] buffer)
         <= new(buffer, 0, buffer.Length);

      object Source = _buffer;

      close()
      {
         _position := _end
      }

      bool write(byte[] buffer, int length)
      {
         if (_end - _position < length) {
            ^ false
         };
   
         Array<byte>.copyTo(_buffer, buffer, _position, length);

         _position += length;

         ^ true
      }
   }

   public sealed ByteArrayReader : BinaryReader
   {
      byte[]         _buffer;
      int            _start;
      int            _end;
      Reference<int> _position;

      constructor new(byte[] buffer, int start, int length)
      {
         _buffer := buffer;
         _start := start;
         _end := start + length;
         _position := start;
      }

      constructor new(byte[] buffer)
         <= new(buffer, 0, buffer.Length);

      object Source = _buffer;

      bool Available = _position < _end;

      close()
      {
         _position := _end
      }

      int read(byte[] array, int actualLength)
      {
         int remaining := _end - _position;
         if (remaining < actualLength) {
            actualLength := remaining
         }
         else remaining := actualLength;

         Array<byte>.copy(array, _buffer, _position, remaining);
      }
   }

   public struct BufferWriter : BinaryWriter
   {
      int  _position; 
      byte _buffer[255];      

      object Source = _buffer;

      bool write(byte[] buffer, int length)
      {
         if (_position >= /*$len _buffer*/255) {
            ^ false
         };
   
         Array<byte>.copyTo(_buffer, buffer, _position, length);

         _position += length;

         ^ true
      }

      close()
      {
         _position := 255
      }

      private bool copyToStream(BinaryWriter externalWriter)
      {
         ^ externalWriter.write(_buffer, _position);
      }

      internal static flush(BufferWriter bufferWriter, BinaryWriter externalWriter)
      {
         bufferWriter.copyToStream(externalWriter);
      }
   }

   public extension bufferWriterOp : BufferWriter
   {
      bool copyToStream(BinaryWriter externalWriter)
      {
         ^ BufferWriter.flush(self, externalWriter);
      }
   }
}