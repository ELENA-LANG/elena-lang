//import extensions;
import extensions'dynamic;
import net;
import system'net;

import chat_shared;

public sealed class ChatClient : BaseClient
{
   TcpClient       _networkClient;
   TcpClientWriter _networkWriter;
    
   string    _address;
   int       _port;
   string    _userName;    
    
   Func                       OnConnect : event; 
   Func<int,object>           OnError : event; 
   Func<string,string,object> OnMessage: event; 

   constructor new()
      <= super new()
   {
   }
    
   constructor new(string ip, int port, string userName)
      <= super new()
   {
      _networkClient := TcpClient.new();
      _networkWriter := TcpClientWriter.new(_networkClient);

      _address := ip;
      _port := port;
      _userName := userName
   }
    
   sendNickname()
   {
      string json := new CommandDTO{ this Command := INTRODUCTION; this Value := _userName; }.toJson();
      int len := json.Length;

      _networkWriter.write(len);
      _networkWriter.write(json);
   }
    
   send(string text)
   {
      string json := new CommandDTO{ this Command := SEND; this Value := text; }.toJson();
      int len := json.Length;
      
      _networkWriter.write(len);
      _networkWriter.write(json);
    }
    
   start()
   {
      _networkClient.onConnect := (TcpClient client)
      {
         OnConnect?.();

         sendNickname()
      };
        
      _networkClient.onError := (TcpClient client, SocketException e)
      {
         OnError?.(e.ErrorCode);
      };
        
      _networkClient.onReceived := (TcpClient client,byte[] buffer, int length)
      {
         self.write(buffer, length);
         var command := self.readCommand();
         if (nil != command)
         {
            (command.Command) =>
               TRANSLATE
               {
                  OnMessage?.(command.User, command.Value)
               }
         };                
      };
        
      _networkClient.connect(_address, _port);
        
      _networkClient.Blocking := false;
      _networkClient.Nodelay := true;        
        
      _networkClient.start()
   }
    
    stop()
   {
       _networkClient.close()
   }
}