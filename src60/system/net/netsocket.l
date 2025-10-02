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

public interface INetListener
{
   abstract start();
   
   abstract start(int backLog);
   
   abstract INetSocket acceptNetSocket();
   
   abstract Task<INetSocket> acceptNetSocketAsync();
}

public interface INetSocketFactory
{
   abstract INetSocket openSocket(string host, short port);

   abstract INetListener openListener(short port, bool noDelay);
         
   Task<INetSocket> openSocketAsync(string host, short port)
      = ::Task<INetSocket>.run({ ^ self.openSocket(host, port); });
      
   Task<INetListener> openListenerAsync(short port, bool noDelay)
      = ::Task<INetListener>.run({ ^ self.openListener(port, noDelay); });
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

public class NetListener : INetListener
{
   NativeSocket   _listenSocket;
   bool           _noDelayMode;
   
   internal constructor open(short port, bool noDelayMode)
   {
      _noDelayMode := noDelayMode;

      //resolve server address and port
      AddrInfo addrinfo := default;

      // set address info
      addrinfo.ai_family := AF_INET;
      addrinfo.ai_socktype := SOCK_STREAM;
      addrinfo.ai_protocol := IPPROTO_TCP;
      addrinfo.ai_flags  := AI_PASSIVE;
        
      // Create a SOCKET for connecting to server
      using(AddrInfoReader reader := new AddrInfoReader(port.toPrintable(), addrinfo)) {
         reader.read(addrinfo);
         int       ai_family := addrinfo.ai_family;
         int       ai_socktype := addrinfo.ai_socktype;
         int       ai_protocol := addrinfo.ai_protocol;
         int       ai_addrlen := addrinfo.ai_addrlen;
         pointer   ai_addrptr := addrinfo.ai_addr;

         _listenSocket := new NativeSocket(ai_family, ai_socktype, ai_protocol);

         _listenSocket.bind(ai_addrptr, ai_addrlen);

         if (_noDelayMode)
            _listenSocket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };      
   }
   
   private Task<NativeSocket> listenAsync()
      = ::Task<NativeSocket>.run({ ^ _listenSocket.accept(); });
   
   start()
   {
      _listenSocket.listen(SOMAXCONN)
   }

   start(int backLog)
   {
      _listenSocket.listen(backLog)
   }
   
   INetSocket acceptNetSocket()
   {
      NativeSocket socket := _listenSocket.accept();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ NetSocket.assign(socket)
   }

   async Task<INetSocket> acceptNetSocketAsync()
   {
      NativeSocket socket := :await listenAsync();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ NetSocket.assign(socket)
   }   
}

public class NativeSocketFactory : INetSocketFactory
{
   constructor create()
   {
   }

   INetSocket openSocket(string host, short port)
      = NetSocket.assign(NativeSocket.connect(host, port));
      
   INetListener openListener(short port, bool noDelay)      
      = NetListener.open(port, noDelay);
}
