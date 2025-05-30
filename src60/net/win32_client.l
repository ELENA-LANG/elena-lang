import system'net;
//import system'io;
//import system'threading;
import extensions;

// --- TcpClient ---

public sealed class TcpClient
{
   NativeSocket        _socket;
   NetworkStream _stream;

   constructor()
   {
   }

   internal constructor new(NativeSocket socket)
   {
      _socket := socket
   }

   constructor connect(string address, int port)
   {
      //resolve server address and port
      AddrInfo addrinfo := default;
        
      // set address info
      addrinfo.ai_family := AF_UNSPEC;
      addrinfo.ai_socktype := SOCK_STREAM;
      addrinfo.ai_protocol := IPPROTO_TCP;
        
      AddrInfoReader reader := new AddrInfoReader(address, port.toString(), addrinfo);
      while (reader.Available)
      {
         reader.read(addrinfo);
            
         int     ai_family := addrinfo.ai_family;
         int     ai_socktype := addrinfo.ai_socktype;
         int     ai_protocol := addrinfo.ai_protocol;
         int     ai_addrlen := addrinfo.ai_addrlen;
         pointer ai_addrptr := addrinfo.ai_addr;

         _socket := new NativeSocket(ai_family, ai_socktype, ai_protocol);

         if (_socket.tryOpen(ai_addrptr, ai_addrlen))
         {
            reader.close();
        
            ^ self
         };
            
         reader.next()
      };
        
      reader.close();
        
      TcpClientException.new("Unable to connect to server").raise()   
   } 

   set Blocking(bool val)
   {
      int mode := val ? 0 : 1;

      mode := _socket.iocommand(FIONBIO, mode)
   }
    
   set Nodelay(bool val)
   {
      _socket.option(IPPROTO_TCP, TCP_NODELAY, val)
   }

   get bool Available()
      = _socket.isConnected();

   private closeInternal()
   {
      _socket.shutdown(SD_BOTH);
      _socket.close();
   }
    
   close()
   {
      if(nil != _socket)
         { closeInternal() }
   }

//   NetworkStream stream()
//       = _stream ?? _stream := NetworkStream.assign(_socket, true);
}

//public sealed class TcpClient
//{
//   Thread    _thread;
//   Socket    _socket;
//
//   Func<TcpClient,object>                 onConnect  : event;
//   Func<TcpClient,byte[],int,object>      onReceived : event;
//   Func<TcpClient,SocketException,object> onError    : event;
//
//   constructor new()
//      <= new(1024);
//    
//   constructor new(int size)
//   {
//      _maxLength := size;
//      _buffer := new byte[](_maxLength)
//   }
//
//   connect(string address, int port)
//   {
//      if(_socket != nil)
//         { InvalidOperationException.raise("The socket is already open") };
//
//      //resolve server address and port
//      AddrInfo addrinfo := default;
//        
//      // set address info
//      addrinfo.ai_family := AF_UNSPEC;
//      addrinfo.ai_socktype := SOCK_STREAM;
//      addrinfo.ai_protocol := IPPROTO_TCP;
//        
//      AddrInfoReader reader := new AddrInfoReader(address, port.toString(), addrinfo);
//      while (reader.Available)
//      {
//         reader.read(addrinfo);
//            
//         int     ai_family := addrinfo.ai_family;
//         int     ai_socktype := addrinfo.ai_socktype;
//         int     ai_protocol := addrinfo.ai_protocol;
//         int     ai_addrlen := addrinfo.ai_addrlen;
//         pointer ai_addrptr := addrinfo.ai_addr;
//
//         _socket := new Socket(ai_family, ai_socktype, ai_protocol);
//
//         if (_socket.tryOpen(ai_addrptr, ai_addrlen))
//         {
//            reader.close();
//                
//            onConnect?.(self);
//        
//            ^ self
//         };
//            
//         reader.next()
//      };
//        
//      reader.close();
//        
//      TcpClientException.new("Unable to connect to server").raise()
//   }
//    
//   private onError(SocketException e)
//   {
//      onError?.(self, e);
//        
//      int  err := e.ErrorCode;
//      if(err == WSAECONNRESET)
//         { closeInternal() }
//   }
//    
//   private readServer()
//   {
//      int received := 0;
//      try
//      {
//         received := _socket.read(_buffer, _maxLength, 0)
//      }
//      catch(SocketException e)
//      {
//         onError(e);
//      };
//
//      if (received == 0)
//      { 
//         closeInternal();
//      }
//      else
//      { 
//         if (received > 0)
//            { onReceived?.(self, _buffer, received) }
//      }
//   }
//    
//   write(byte[] buffer, int length)
//   {
//      int sent := _socket.write(buffer, length, 0)
//   }
//    
//   private proceed()
//   {
//      bool retVal := false;
//      if (nil != _socket)
//         { readServer(); retVal := true };
//        
//      ^ retVal
//   }
//    
//   start()
//   {
//      _thread := Thread.assign(
//      {
//         while(proceed()){}
//      });
//        
//      _thread.start()
//   }
//}
