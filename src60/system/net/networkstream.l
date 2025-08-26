import system;
import system'io;
import system'text;
import system'threading;

public class NetworkStream : Stream
{
   INetSocket  _socket;
   bool        _ownStream;
   int         _timeout;
   int         _counter;

   constructor(INetSocket socket)
   {
      _socket := socket;
      _ownStream := false;
      
      _timeout := 100;
      _counter := 2;
   }

   constructor()
   {
      _ownStream := false;
      
      _timeout := 300;
      _counter := 3;
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

   indexed internal Task<int> readAsync(MemoryBuffer buffer)
      = ::Task<int>.run(
         { 
            byte dump[512];            
            int received := _socket.read(dump, 512);
            
            buffer.write(0, received, dump);
         
            ^ received
         });

   int write(byte[] dump, int length)
   {
      int retVal := _socket.write(dump, length);

      ^ retVal
   }
     
   indexed internal Task<int> writeAsync(byte[] dump, int length)
      = _socket.writeAsync(dump, length);
     
   indexed get internal bool isDataAvailable()
   {
//      ^ _socket.AvailableToRead

      if (_socket.AvailableToRead) { ^ true; };
         
      int index := _counter;
      while (index > 0) {
         Thread.sleep(_timeout);
         index--;
         
         if (_socket.AvailableToRead) { ^ true; };
      };
      
      ^ false;
   }

   indexed async internal Task writeStringAsync(string s)
   {
      byte[] tmp := new byte[](1024);
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

   indexed async internal Task reaAllAsync(MemoryBuffer buffer, bool nonBlocking)
   {
      bool skipCheck := !nonBlocking;
      
      while (true) {
         int received := 0;
         if (skipCheck || self.isDataAvailable)
            received := :await readAsync(buffer);
         
         if (received == 0)
            :break;         
      };
   }

   indexed async internal Task<string> readAsStringAsync(bool nonBlocking)
   {
      bool skipCheck := !nonBlocking;
      
      MemoryBuffer buffer := MemoryBuffer.allocate();
      while (true) {
         int received := 0;
         if (skipCheck || self.isDataAvailable)
            received := :await readAsync(buffer);
         
         if (received == 0)
            :break;         
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

   Task writeStringAsync(string s)
      = self.writeStringAsync(s);

   writeString(string s)
      = self.writeString(s);

   Task<int> readAsync(MemoryBuffer buffer)
      = self.readAsync(buffer);

   Task readAllAsync(MemoryBuffer buffer, bool nonBlocking)
      = self.readAllAsync(buffer, nonBlocking);

   Task<string> readAllAsStringAsync(bool nonBlocking)
      = self.readAllAsStringAsync(nonBlocking);

   bool isDataAvailable
      = self.isDataAvailable;
}
