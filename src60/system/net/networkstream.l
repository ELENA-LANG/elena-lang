import system;
import system'io;
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
     
   write(byte[] dump, int length)
   {
      _socket.send(dump, length, 0);
   }
     
   indexed internal Task<int> readAsync(byte[] dump, int length)
      = _socket.receiveAsync(dump, length, 0);
     
   indexed internal Task writeAsync(byte[] dump, int length)
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

   Task writeAsync(byte[] dump, int length)
      = self.writeAsync(dump, length);

   bool isDataAvailable
      = self.isDataAvailable;
}
