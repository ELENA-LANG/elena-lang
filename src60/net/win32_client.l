import system'net;
import system'io;
import system'threading;
import extensions;

// --- TcpClient ---

public sealed class TcpClient
{
   Thread    _thread;
   Socket    _socket;
   byte[]    _buffer;
   int       _maxLength;    
   object    _lock;

   Func<TcpClient,object>                 onConnect  : event;
   Func<TcpClient,byte[],int,object>      onReceived : event;
   Func<TcpClient,SocketException,object> onError    : event;

   constructor new()
      <= new(1024);
    
   constructor new(int size)
   {
      _lock := new Object();
        
      _maxLength := size;
      _buffer := new byte[](_maxLength)
   }

   connect(string address, int port)
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

         _socket := new Socket(ai_family, ai_socktype, ai_protocol);

         if (_socket.tryOpen(ai_addrptr, ai_addrlen))
         {
            reader.close();
                
            onConnect?.(self);
        
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

      mode := _socket.iocommand(FIONBIO)
   }
    
   set Nodelay(bool val)
   {
      _socket.option(IPPROTO_TCP, TCP_NODELAY, val)
   }
    
   private closeInternal()
   {
      _socket.shutdown(SD_BOTH);
      _socket.close();
        
      _socket := nil
   }
    
   close()
   {
      lock (_lock)
      {
         if(nil != _socket)
            { closeInternal() }
      }
   }
    
   private onError(SocketException e)
   {
      onError?.(self, e);
        
      int  err := e.ErrorCode;
      if(err == WSAECONNRESET)
         { closeInternal() }
   }
    
   private readServer()
   {
      int received := 0;
      try
      {
         received := _socket.read(_buffer, _maxLength, 0)
      }
      catch(SocketException e)
      {
         onError(e)
      };
        
      if (received == 0)
      { 
         closeInternal() 
      }
      else
      { 
         if (received > 0)
            { onReceived?.(self, _buffer, received) }
      }
   }
    
   write(byte[] buffer, int length)
   {
      int sent := _socket.write(buffer, length, 0)
   }
    
   private proceed()
   {
      bool retVal := false;
      lock (_lock)
      {
         if (nil != _socket)
            { readServer(); retVal := true }
      };
        
      ^ retVal
   }
    
   start()
   {
      _thread := Thread.assign(
      {
            while(proceed()){}
      });
        
      _thread.start()
   }
}

public class TcpClientWriter : BinaryWriter
{
   TcpClient _client;

   constructor new(TcpClient client)
   {
      _client := client;
   }

   object Source = _client;
   
   bool write(byte[] buffer, int length)
   {
      _client.write(buffer, length);
   
      ^ true
   }
   
   close()
   {
      _client.close()
   }   
}
