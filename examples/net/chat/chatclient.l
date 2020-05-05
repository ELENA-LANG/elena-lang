import extensions;
import extensions'dynamic;
import net;

sealed class ChatClient : BaseClient
{
    TcpClient theNetworkClient;
    
    string    theAddress;
    int       thePort;
    string    theUserName;    
    
    constructor new()
        <= new()
    {
    }
    
    constructor new(string ip, int port, string userName)
        <= new()
    {
        theNetworkClient := TcpClient.new();
        
        theAddress := ip;
        thePort := port;
        theUserName := userName
    }
    
    sendNickname()
    {
        string json := new CommandDTO{ this Command := INTRODUCTION; this Value := theUserName; }.toJson();
        int len := json.Length;
        
        theNetworkClient.write(len).write(json);
    }
    
    send(string text)
    {
        string json := new CommandDTO{ this Command := SEND; this Value := text; }.toJson();
        int len := json.Length;
        
        theNetworkClient.write(len).write(json)
    }
    
    start()
    {
        theNetworkClient.onConnect := (TcpClient client)
        {
            console.writeLine:"connected";
            
            self.sendNickname()
        };
        
        theNetworkClient.onError := (TcpClient client, Exception e)
        {
            console.printLine("Exception #",e.error_code);
        };
        
        theNetworkClient.onReceived := (TcpClient client,byte[] buffer, int length)
        {
            self.write(buffer, length);
            var command := self.readCommand();
            if (nil != command)
            {
                (command.Command) =>
                    TRANSLATE
                    {
                        console.printLine().printLine(command.User,":",command.Value).print:">"
                    }
            };                
        };
        
        theNetworkClient.connect(theAddress, thePort);
        
        theNetworkClient.Blocking := false;
        theNetworkClient.Nodelay := true;        
        
        theNetworkClient.start()
    }
    
    stop()
    {
        theNetworkClient.close()
    }
}