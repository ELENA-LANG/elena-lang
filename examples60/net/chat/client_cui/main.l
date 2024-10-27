import extensions;
import chat_client;

class Client
{
   ChatClient _chatClient;

   start(string userName, string ip, int port)
   {
      _chatClient := ChatClient.new(ip, port, userName);
        
      _chatClient.OnError := (int err) { Console.printLine("Exception #",err); };
      _chatClient.OnConnect := { Console.writeLine("connected"); };
      _chatClient.OnMessage := (string user, string message){ Console.printLine().printLine(user,":",message).print(">") };

      _chatClient.start()      
   }

   send(string message)
   { 
      _chatClient.send(message);
   }

   stop()
   { 
      _chatClient.stop();
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
      Console.printLine("ELENA Chat Client v.", VERSION);
        
      string userName := Console.write("Enter nickname:").readLine();

      try {
         _client.start(userName, _ipAddress, _port);
      }
      catch(Exception ex) {
         Console.writeLine("Cannot esteblish connection!"); AbortException.raise()
      };
            
      ^ "Enter -q to exit the chat"
   }
   
    string Prompt
        = ">";
    
    proceed(line)
    {
        if(line == "-q")
        {
            _client.stop();
    
            forward program.stop()
        }
        else
        {
            _client.send(line)
        };

        ^ EmptyString
    }
}

public static control = new Controller();