import system'net;
import system'threading;

public class TcpListener
{
   Socket _listenSocket;
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

         _listenSocket := new Socket(ai_family, ai_socktype, ai_protocol);

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

   Socket acceptSocket()
   {
      Socket socket := _listenSocket.accept();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ socket
   }

   async Task<Socket> acceptSocketAsync()
   {
      Socket socket := :await _listenSocket.acceptAsync();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ socket
   }

   TcpClient acceptTcpClient()
   {
      Socket socket := _listenSocket.accept();
      if:not(socket.isInvalid)
      {
         if (_noDelayMode)
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
      };

      ^ TcpClient.new(socket)
   }

   async Task<TcpClient> acceptTcpClientAsync()
   {
      Socket socket := :await _listenSocket.acceptAsync();
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

   close()
      <= stop();
}