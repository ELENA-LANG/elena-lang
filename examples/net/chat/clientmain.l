import system'dynamic;

class Client
{
    ChatClient theClient;
    
    start(string userName, string ip, int port)
    {
        theClient := ChatClient.new(ip, port, userName);
        
        theClient.start()
    }
    
    send(text)
        => theClient;
    
    stop()
    {
        theClient.stop()
    }
}

public control = new Client().mixInto(new
{
    greeting()
    {
        console.writeLine:"ELENA Char Client";
        
        var userName := console.write:"Enter nickname:".readLine();
        
        __target.start(userName, "127.0.0.1", DEFAULT_PORT)
                    | on:(e){ console.writeLine:"Cannot esteblish connection!"; AbortException.raise() };
            
        ^ "Enter -q to exit the chat"
    }
    
    string Prompt
        = ">";
    
    proceed(line)
    {
        if(line == "-q")
        {
            __target.stop();
    
            forward program.stop()
        }
        else
        {
            __target.send(line)
        };

        ^ emptyString
    }
});
