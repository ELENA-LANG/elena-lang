import system'net;
//import system'io;
import system'threading;
import extensions;

// --- TcpClient ---

public sealed class TcpClient
{
   NativeSocket  _socket;
   NetworkStream _stream;

   constructor()
   {
   }

   internal constructor new(NativeSocket socket)
   {
      _socket := socket
   }

   constructor connect(string host, int port)
   {
      _socket := NativeSocket.connect(host, port);
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

   NetworkStream stream()
      = _stream ?? _stream := NetworkStream.assign(_socket, true);
}

public sealed class EventBasedTcpClient
{
   Thread    _thread;
   TcpClient _client;
   bool      _running;
   byte[]    _buffer;
   int       _maxLength;    

   Func<TcpClient>                   onConnect  : event;
   Action<TcpClient,byte[],int>      onReceived : event;
   Action<TcpClient,SocketException> onError    : event;

   constructor new()
      <= new(1024);
    
   constructor new(int size)
   {
      _running := false;
      _maxLength := size;
      _buffer := new byte[](_maxLength)
   }

   connect(string host, int port)
   {
      _client := TcpClient.connect(host, port);
      _client.Blocking := true;
      
      onConnect?.(_client);
   }
    
   private onError(SocketException e)
   {
      onError?.(self, e);
        
      int  err := e.ErrorCode;
      if(err == WSAECONNRESET)
         { closeInternal() }
   }
    
   int write(byte[] buffer, int length)
   {
      auto stream := _client.stream();
      
      ^ stream.write(buffer, length)
   }
    
   private proceed(NetworkStream stream)
   {
      int received := 0;
      try
      {
         received := stream.read(_buffer, _maxLength)
      }
      catch(SocketException e)
      {
         onError(e);
      };

      if (received == 0)
      { 
         closeInternal();
      }
      else
      { 
         if (received > 0)
            { onReceived?.(self, _buffer, received) }
      }
   }
    
   start()
   {
      _running := true;
      
      _thread := Thread.assign(
      {
         auto stream := _client.stream();
         while(_running)
         {
            proceed(stream);
         }
      });
        
      _thread.start()
   }
   
   private closeInternal()
   {
      _client.close();
      _running := false;
   }
}
