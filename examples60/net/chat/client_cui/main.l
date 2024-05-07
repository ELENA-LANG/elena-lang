import extensions;
import chat_client;

class Client
{
   ChatClient _chatClient;

   start(string userName, string ip, int port)
   {
      _chatClient := ChatClient.new(ip, port, userName);
        
      _chatClient.OnError := (int err) { console.printLine("Exception #",err); };
      _chatClient.OnConnect := { console.writeLine("connected"); };
      _chatClient.OnMessage := (string user, string message){ console.printLine().printLine(user,":",message).print(">") };

      _chatClient.start()      
   }
}

class Controller
{
   Client _client;
   string _ipAddress;
   int    _port;

   constructor()
   {
      _client := new Client();
      _ipAddress := "127.0.0.1";
      _port := DEFAULT_PORT;
   }

   greeting()
   {
      console.printLine("ELENA Chat Client v.", VERSION);
        
      string userName := console.write("Enter nickname:").readLine();

      try {
         _client.start(userName, _ipAddress, _port);
      }
      catch(Exception ex) {
         console.writeLine("Cannot esteblish connection!"); AbortException.raise()
      };
            
      ^ "Enter -q to exit the chat"
   }
   
}

public static control = new Controller();