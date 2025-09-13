import system'calendar;
import system'net;
import system'threading;

public class TcpListener : interface<INetListener>
{
   NativeSocket _listenSocket;
   bool   _noDelayMode;

   constructor new(int port)
      <= new(port, true);

   constructor new(int port, bool noDelayMode)
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

   stop()
   {
      _listenSocket.shutdown(SD_BOTH);
        
      _listenSocket.close()
   }

   NativeSocket acceptSocket()
   {
      NativeSocket socket := _listenSocket.accept();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ socket
   }

   private Task<NativeSocket> listenAsync()
      = ::Task<NativeSocket>.run({ ^ _listenSocket.accept(); });

   async Task<NativeSocket> acceptSocketAsync()
   {
      NativeSocket socket := :await listenAsync();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ socket
   }

   TcpClient acceptTcpClient()
   {
      NativeSocket socket := _listenSocket.accept();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ TcpClient.new(socket)
   }

   async Task<TcpClient> acceptTcpClientAsync()
   {
      NativeSocket socket := :await listenAsync();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ TcpClient.new(socket)
   }

   start()
   {
      _listenSocket.listen(SOMAXCONN)
   }

   start(int backLog)
   {
      _listenSocket.listen(backLog)
   }

   close()
      <= stop();

   bool Pending
      = _listenSocket.poll(Timespan.Default, SelectMode.SelectRead);
}
