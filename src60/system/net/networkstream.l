import system;
import system'io;
import system'text;
import system'threading;

public class NetworkStream : Stream
{
   INetSocket  _socket;
   bool        _ownStream;
   byte[]      _buffer; 

   constructor(INetSocket socket)
   {
      _socket := socket;
      _ownStream := false;
      _buffer := new byte[](512);
   }

   constructor assign(INetSocket socket, bool socketStream)
   {
      _socket := socket;
      _ownStream := socketStream;
   }

   int Length
   {
      get()
      {
         NotSupportedException.raise();

         ^ 0 // to make compiler happy
      }
   }
     
   int Index
   {
      get()
      {
         NotSupportedException.raise();

         ^ 0 // to make compiler happy
      }
         
      set(retVal)
      {
         NotSupportedException.raise()
      }
   }

   int read(byte[] dump, int length)
      = _socket.read(dump, length);
     
   int write(byte[] dump, int length)
   {
      int retVal := _socket.write(dump, length);

      ^ retVal
   }
     
   indexed internal Task<int> readAsync(byte[] dump, int length)
      = _socket.readAsync(dump, length);
     
   indexed internal Task<int> writeAsync(byte[] dump, int length)
      = _socket.writeAsync(dump, length);
     
   indexed internal bool isDataAvailable
      = _socket.AvailableToRead;

   indexed async internal Task<int> readAsync(MemoryBuffer buffer, int length)
   {
      int total_received := 0;
      int total := length;
      while (total > 0) {
         int size := 512;
         if (size > length)
            size := length;

         int received := :await readAsync(_buffer, size);
         if (received > 0) {
            total_received += received;
            total -= received;

            buffer.write(0, received, _buffer);
         }
         else :break;
      };

      ^ total_received;
   }

   indexed internal int readToBuffer(MemoryBuffer buffer, int length)
   {
      int total_received := 0;
      int total := length;
      while (total > 0) {
         int size := 512;
         if (size > length)
            size := length;

         int received := read(_buffer, size);
         if (received > 0) {
            total_received += received;
            total -= received;

            buffer.write(0, received, _buffer);
         }
         else :break;
      };

      ^ total_received;
   }

   indexed async internal Task writeAsync(string s)
   {
      byte tmp[1024];
      int len := s.Length;
      int index := 0;
      while (len > 0) {
         int sublen := 1024;
         if (len < sublen) {
            sublen := len;
         };

         int converted := UTF8Encoder.toByteArray(s, index, ref sublen, tmp, 0, 1024);
         
         converted := :await self.writeAsync(tmp, converted);

         index += converted;
         len -= converted;
      };
   }

   indexed async internal Task<string> readAsStringAsync()
   {
      MemoryBuffer buffer := MemoryBuffer.allocate();

      byte tmp[1024];
      int read := :await self.readAsync(tmp, 1024);
      while (read > 0) {
         buffer.write(0, read, tmp);

         read := :await self.readAsync(tmp, 1024);
      };

      ^ UTF8Encoding.toString(0, buffer.Length, buffer.Value);
   }

   close()
   {
      if (_ownStream)
         InvalidOperationException.raise();

      _socket.close()
   }
   
}

public extension AsyncStreamExtension : NetworkStream
{
   Task<int> readAsync(byte[] dump, int length)
      = self.readAsync(dump, length);

   Task<int> writeAsync(byte[] dump, int length)
      = self.writeAsync(dump, length);

   Task writeAsync(string s)
      = self.writeAsync(s);

   Task<string> readAsStringAsync()
      = self.readAsStringAsync();

   Task<int> readAsync(MemoryBuffer buffer, int length)
      = self.readAsync(buffer, length);

   int readToBuffer(MemoryBuffer buffer, int length)
      = self.readToBuffer(buffer, length);

   bool isDataAvailable
      = self.isDataAvailable;
}
