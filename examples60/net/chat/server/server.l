import system'threading;
import system'net;
import extensions'dynamic;
import net;

import chat_shared;

public sealed ChatServer
{
   object          _lock; 

   TcpServer       _networkServer;
   TcpServerWriter _networkWriter;

   Thread          _broadcastThread;

   Func                         OnConnect : event;
   Func<string, object>         OnIntro   : event;
   Func<string, string, object> OnMessage : event;
   Func<string, object>         OnLeave   : event;
   Func<int, object>            OnError   : event;


   constructor new(int port)
   {
      _lock := new object();

      _networkServer := TcpServer.new(port);
      _networkServer.Blocking := false;

      _networkWriter := TcpServerWriter.new(_networkServer);
   }

   private bool proceed()
   {
      try {
         foreach(TcpRemoteClient client; in _networkServer.Clients) {
            lock(_lock) {
               if (client.Parameter; is ChatMember member)
               {
                  var command := member.readCommand();
                  if (command != nil) {
                     (command.Command) =>
                           INTRODUCTION:
                           {
                              member.Name := command.Value;
      
                              OnIntro?.(member.Name);
                           }
                           SEND:
                           {
                              OnMessage?.(member.Name, command.Value);
                              
                              member.Server.translate(new CommandDTO
                                                              { 
                                                                  this Command := TRANSLATE; 
                                                                  this Value := command.Value; 
                                                                  this User := member.Name; 
                                                              })
                           }

                  };
               }
            }
         };
      }
      catch(Exception ex) {
         Console.writeLine(ex);
      };

      ^ true
   }

   translate(command)
   {
      string json := command.toJson();
        
      _networkWriter.write(json.Length);
      _networkWriter.write(json)
   }
    
   start()
   {
      _networkServer.OnConnect := (TcpRemoteClient client)
      {
         lock (_lock)
         {
            OnConnect?.();
            
            client.Parameter := ChatMember.new(client, self)
         }                                
      };
        
      _networkServer.OnLeave := (TcpRemoteClient client)
      {
         lock (_lock)
         {
            OnLeave?.(client.Parameter.Name ?? "client");
         }
      };
        
      _networkServer.OnError := (TcpRemoteClient client, SocketException e)
      {
         lock (_lock)
         {
            OnError?.(e.ErrorCode)
         }
      };
        
      _networkServer.OnReceived := (TcpRemoteClient client, byte[] data, int length)
      {
         lock (_lock)
         {
            (client.Parameter as::ChatMember).write(data, length)
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