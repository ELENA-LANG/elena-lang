import system'threading;
import system'net;
import net;

import chat_shared;

public sealed ChatServer
{
   object    _lock; 

   TcpServer _networkServer;
   Thread    _broadcastThread;

   Func OnConnect                   : event;
   Func<BaseClient, object> OnLeave : event;
   Func<int, object>        OnError : event;

   constructor new(int port)
   {
      _lock := new object();

      _networkServer := TcpServer.new(port);
      _networkServer.Blocking := false
   }

   private bool proceed()
   {
      foreach(TcpRemoteClient client; in _networkServer.Clients) {
         lock(_lock) {
            
         }
      };

      ^ true
   }

   start()
   {
      _networkServer.OnConnect := (TcpRemoteClient client)
      {
         lock (_lock)
         {
            //console.writeLine:"new client joint";
            OnConnect?.();
            
            client.Parameter := ChatMember.new(client, self)
         }                                
      };
        
      _networkServer.OnLeave := (TcpRemoteClient client)
      {
         lock (_lock)
         {
            //console.writeLine:"client left"
            OnLeave?.(client.Parameter)
         }
      };
        
      _networkServer.OnError := (TcpRemoteClient client, SocketException e)
      {
         lock (_lock)
         {
            //console.printLine("exception #",e.ErrorCode)
            OnError?.(e.ErrorCode)
         }
      };
        
      _networkServer.OnReceived := (TcpRemoteClient client, byte[] data, int length)
      {
         lock (_lock)
         {
            cast ChatMember(client.Parameter).write(data, length)
         }                        
      };

      _networkServer.start();

      _broadcastThread := Thread.assign(
      {
         while (proceed()) {}
      });

      _broadcastThread.start()
   }
}