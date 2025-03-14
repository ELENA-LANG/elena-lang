import system;
import system'io;
import system'text;
import system'threading;

public class NetworkStream : Stream
{
   Socket _socket;
   bool   _ownStream;

   constructor(Socket socket)
   {
      _socket := socket;
      _ownStream := false;
   }

   constructor assign(Socket socket, bool socketStream)
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
      = _socket.receive(dump, length, 0);
     
   int write(byte[] dump, int length)
   {
      int retVal := _socket.send(dump, length, 0);

      ^ retVal
   }
     
   indexed internal Task<int> readAsync(byte[] dump, int length)
      = _socket.receiveAsync(dump, length, 0);
     
   indexed internal Task<int> writeAsync(byte[] dump, int length)
      = _socket.sendAsync(dump, length, 0);
     
   indexed internal bool isDataAvailable
      = _socket.available() != 0;

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

   async Task writeAsync(string s)
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

   async Task<string> readAsStringAsync()
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

   bool isDataAvailable
      = self.isDataAvailable;
}
