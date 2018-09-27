import system'dynamic.

const int DEFAULT_PORT = 6881.

class Client
{
    T<ChatClient> theClient.
    
    start(literal userName, literal ip, int port)
    [
        theClient := ChatClient new(ip, port, userName).
        
        theClient start.
    ]
    
    send(text)
        => theClient.
    
    stop
    [
        theClient stop.
    ]
}

public control = Client new; mixInto:
{
    greeting
    [
        console writeLine:"ELENA Char Client".
        
        var userName := console write:"Enter nickname:"; readLine.
        
        __target start(userName, "127.0.0.1", DEFAULT_PORT)
            | if(:e)[ console writeLine:"Cannot esteblish connection!". AbortException new; raise ].
            
        ^ "Enter -q to exit the chat"
    ]
    
    prompt
        = ">".
    
    proceed : line
    [
        if(line == "-q")
            [
                __target stop.
        
                forwards'program stop.
            ];
            [
                __target send(line).
            ].
            
        ^ emptyLiteral.
    ]
}.
