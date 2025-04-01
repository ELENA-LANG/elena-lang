import system'threading;

public interface INetSocket
{
   Task<int> readAsync(byte[] buffer, int length)
      = ::Task<int>.run({ ^ self.read(buffer, length) });
     
   Task<int> writeAsync(byte[] buffer, int length)
      = ::Task<int>.run({ ^ self.write(buffer, length) });     

   abstract int read(byte[] buffer, int length);

   abstract int write(byte[] buffer, int length);

   get abstract bool AvailableToRead();

   verify() {}

   abstract close();
}

public interface INetSocketFactory
{
   abstract INetSocket openSocket(string host, short port);

   Task<INetSocket> openSocketAsync(string host, short port)
      = ::Task<INetSocket>.run({ ^ self.openSocket(host, port); });
}

public class NetSocket : INetSocket
{
   NativeSocket _socket;

   internal constructor assign(NativeSocket socket)
   {
      _socket := socket
   }

   int read(byte[] buffer, int length)
      = _socket.receive(buffer, length, 0);

   int write(byte[] buffer, int length)
      = _socket.send(buffer, length, 0);

   Task<int> readAsync(byte[] buffer, int length)
      = ::Task<int>.run({ ^ self.read(buffer, length) });
     
   Task<int> writeAsync(byte[] buffer, int length)
      = ::Task<int>.run({ ^ self.write(buffer, length) });     

   get bool AvailableToRead
      = _socket.available() != 0;

   close()
      => _socket;
}

public class NativeSocketFactory : INetSocketFactory
{
   constructor create()
   {
   }

   INetSocket openSocket(string host, short port)
      = NetSocket.assign(NativeSocket.connect(host, port));
}

