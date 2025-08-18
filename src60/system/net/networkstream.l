import system;
import system'io;
import system'text;
import system'threading;

public class NetworkStream : Stream
{
   INetSocket  _socket;
   bool        _ownStream;

   constructor(INetSocket socket)
   {
      _socket := socket;
      _ownStream := false;
   }

   constructor()
   {
      _ownStream := false;
   }

   constructor assign(INetSocket socket, bool socketStream)
   {
      _socket := socket;
      _ownStream := socketStream;
   }

   constructor assign(NativeSocket socket, bool socketStream)
      <= assign(NetSocket.assign(socket), socketStream);

   int Length
   {
      get()
      {
         ^ NotSupportedException.raise();
      }
   }
     
   int Index
   {
      get()
      {
         ^ NotSupportedException.raise();
      }
         
      set(retVal)
      {
         NotSupportedException.raise()
      }
   }

   int read(byte[] dump, int length)
      = _socket.read(dump, length);
     
   indexed internal Task<int> readAsync(byte[] dump, int length)
      = _socket.readAsync(dump, length);
     
   indexed async internal Task readExactAsync(MemoryBuffer buffer, int length)
   {
      int total := length;
      while (total > 0) {
         byte dump[512];
         int page_size := 512;
         if (page_size < total)
            page_size := total;
         
         int received := :await readAsync(dump, page_size);
         total -= received;
         
         buffer.write(0, received, dump);
      }
   }
     
   indexed async internal Task readAllAsync(MemoryBuffer buffer, bool nonBlocking)
   {
      bool skipCheck := !nonBlocking;
      
      byte dump[512];
      while (true) {
         int received := 0;
         if (skipCheck || _socket.AvailableToRead)
            received := :await readAsync(dump, 512);
         
         buffer.write(0, received, dump);
         
         if (received == 0)
            :break;
      }
   }

   int write(byte[] dump, int length)
   {
      int retVal := _socket.write(dump, length);

      ^ retVal
   }
     
   indexed internal Task<int> writeAsync(byte[] dump, int length)
      = _socket.writeAsync(dump, length);
     
   indexed internal bool isDataAvailable
      = _socket.AvailableToRead;

   indexed async internal Task writeStringAsync(string s)
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

   indexed internal writeString(string s)
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
         
         converted := self.write(tmp, converted);

         index += converted;
         len -= converted;
      };
   }

   indexed async internal Task<string> readAsStringAsync(bool nonBlocking)
   {
      MemoryBuffer buffer := MemoryBuffer.allocate();
      :await readAllAsync(buffer, nonBlocking);

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

   Task writeStringAsync(string s)
      = self.writeStringAsync(s);

   writeString(string s)
      = self.writeString(s);

   Task<string> readAsStringAsync()
      = self.readAsStringAsync();

   Task readExactAsync(MemoryBuffer buffer, int length)
      = self.readExactAsync(buffer, length);

   Task readAllAsync(MemoryBuffer buffer, bool nonBlocking)
      = self.readAllAsync(buffer, nonBlocking);

   bool isDataAvailable
      = self.isDataAvailable;
}
