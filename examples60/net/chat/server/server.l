import system'threading;
import system'net;
import extensions'dynamic;
import net;

import chat_shared;

public sealed ChatServer
{
   object    _lock; 

   TcpServer _networkServer;
   Thread    _broadcastThread;

   Func                         OnConnect : event;
   Func<object, object>         OnIntro   : event;
   Func<object, object, object> OnMessage : event;
   Func<BaseClient, object>     OnLeave   : event;
   Func<int, object>            OnError   : event;


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
            if (client.Parameter; is ChatMember member)
            {
               var command := member.readCommand();
               (command.Command) =>
                     INTRODUCTION
                     {
                        member.Name := command.Value;

                        OnIntro?.(member);

//                        console.printLine(command.Value," joined")
                     }
                     SEND
                     {
                        OnMessage?.(member, command.Value);
                        
//                        console.printLine(member.Name,":",command.Value).
                                
                        member.Server.translate(new CommandDTO
                                                        { 
                                                            this Command := TRANSLATE; 
                                                            this Value := command.Value; 
                                                            this User := member.Name; 
                                                        })
                     };
            }
         }
      };

      ^ true
   }

   translate(command)
   {
      string json := command.toJson();
        
      _networkServer.sendToAll(json.Length).sendToAll(json)
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
            (client.Parameter :as ChatMember).write(data, length)
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