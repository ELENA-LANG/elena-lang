import system'collections;
import system'net;
import system'threading;
import system'text;
import system'io;

public sealed class TcpRemoteClient
{
   Socket _socket;
    
   object Parameter : prop;
    
   constructor assign(Socket socket)
   {
      _socket := socket
   }
    
   int read(byte[] buffer, int maxLength)
   {
      ^ _socket.read(buffer, maxLength, 0)
   }
    
   int write(byte[] buffer, int length)
   {
      ^ _socket.write(buffer, length, 0);
   }
    
   close()
      => _socket;
}

public sealed class TcpServer
{
   Socket                _listenSocket;
   List<TcpRemoteClient> _clients;
   Thread                _listenThread;  
   byte[]                _buffer;
   int                   _maxLength;

   Func<TcpRemoteClient,object> OnConnect               : event;
   Func<TcpRemoteClient,object> OnLeave                 : event;
   Func<TcpRemoteClient,byte[],int,object> OnReceived   : event;
   Func<TcpRemoteClient,SocketException,object> OnError : event;
    
   constructor new(int port)
      <= new(port, 1024);

   constructor new(int port, int bufferSize)
   {
      _maxLength := bufferSize;
      _buffer := new byte[](_maxLength);

      _clients := new List<TcpRemoteClient>();
        
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

      };

      _listenSocket.listen(SOMAXCONN)
   }

   Enumerable<TcpRemoteClient> Clients = new Enumerable<TcpRemoteClient>
   { 
      Enumerator<TcpRemoteClient> enumerator() => _clients; 
   };

   set Blocking(bool val)
   {
      int mode := val ? 0 : 1;

      mode := _listenSocket.iocommand(FIONBIO)
   }    
    
   private TcpRemoteClient newClient()
   {
      auto socket := _listenSocket.accept();
      TcpRemoteClient client := nil;
      ifnot(socket.isInvalid)
      {
         socket.option(IPPROTO_TCP, TCP_NODELAY, true);

         client := TcpRemoteClient.assign(socket);

         _clients.append(client)
      };

      ^ client
   }
    
   private closeClient(TcpRemoteClient client, int index)
   {
      _clients.remove(index);
        
      client.close();

      OnLeave?.(client)
   }
    
   private handleError(SocketException e, TcpRemoteClient client, int n)
   {
      OnError?.(client, e);
        
      int err := e.ErrorCode;
      if (err == WSAECONNRESET)
         { closeClient(client, n) }
   }
    
   private readClients()
   {
      int counter := _clients.Length;
      for (int i := 0; i < counter; i += 1) {
         TcpRemoteClient client := _clients[i];

         int rcvd := 0;
         try
         {
            rcvd := client.read(_buffer, _maxLength)
         }
         catch(SocketException e)
         {
            handleError(e, client, i)
         };
            
         if (rcvd == 0)
         { 
            closeClient(client, i);
            $break;  
         }
         else
         { 
            if (rcvd > 0)
            {
               OnReceived?.(client,_buffer,rcvd)
            }
         }
      }
   }
    
   private bool proceed()
   {
      auto client := newClient();
      if (nil != client)
      {
         OnConnect?.(client)
      };

      readClients();
        
      ^ true
   }
    
   start()
   {
      _listenThread := Thread.assign(
      {
         while (proceed()){}
      });
        
      _listenThread.start()
   }
    
   TcpServer broadcast(byte[] buffer, int length)
   {
      int counter := _clients.Length;
        
      for(int i := 0; i < counter; i += 1)
      {
         TcpRemoteClient client := _clients[i];
            
         client.write(buffer, length)
      }
   }
    
   close()
   {
      _listenSocket.shutdown(SD_BOTH);
        
      _listenSocket.close()
   }
}

public class TcpServerWriter : BinaryWriter
{
   TcpServer _server;

   constructor new(TcpServer server)
   {
      _server := server;
   }

   object Source = _server;
   
   bool write(byte[] buffer, int length)
   {
      _server.broadcast(buffer, length);
   
      ^ true
   }
   
   close()
   {
      _server.close()
   }   
}

