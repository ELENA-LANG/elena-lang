//import extensions;
//import extensions'dynamic;
import net;

import chat_shared;

public sealed class ChatClient : BaseClient
{
   TcpClient _networkClient;
    
   string    _address;
   int       _port;
   string    _userName;    
    
   constructor new()
      <= super new()
   {
   }
    
   constructor new(string ip, int port, string userName)
      <= super new()
   {
      _networkClient := TcpClient.new();

      _address := ip;
      _port := port;
      _userName := userName
   }
    
//    sendNickname()
//    {
//        string json := new CommandDTO{ this Command := INTRODUCTION; this Value := theUserName; }.toJson();
//        int len := json.Length;
//        
//        theNetworkClient.write(len).write(json);
//    }
//    
//    send(string text)
//    {
//        string json := new CommandDTO{ this Command := SEND; this Value := text; }.toJson();
//        int len := json.Length;
//        
//        theNetworkClient.write(len).write(json)
//    }
//    
//    start()
//    {
//        theNetworkClient.onConnect := (TcpClient client)
//        {
//            console.writeLine:"connected";
//            
//            self.sendNickname()
//        };
//        
//        theNetworkClient.onError := (TcpClient client, Exception e)
//        {
//            console.printLine("Exception #",e.error_code);
//        };
//        
//        theNetworkClient.onReceived := (TcpClient client,byte[] buffer, int length)
//        {
//            self.write(buffer, length);
//            var command := self.readCommand();
//            if (nil != command)
//            {
//                (command.Command) =>
//                    TRANSLATE
//                    {
//                        console.printLine().printLine(command.User,":",command.Value).print:">"
//                    }
//            };                
//        };
//        
//        theNetworkClient.connect(theAddress, thePort);
//        
//        theNetworkClient.Blocking := false;
//        theNetworkClient.Nodelay := true;        
//        
//        theNetworkClient.start()
//    }
//    
//    stop()
//    {
//        theNetworkClient.close()
//    }
}