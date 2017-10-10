import system'dynamic.

const int DEFAULT_PORT = 6881.

class Client
{
    object theClient.
    
    start name:userName ip:ip port:port
    [
        theClient := ChatClient new ip:ip port:port userName:userName.
        
        theClient start.
    ]
    
    send text:text
        => theClient.
    
    stop
    [
        theClient stop.
    ]
}

symbol control = Client new; mixInto:
{
    greeting
    [
        console writeLine:"ELENA Char Client".
        
        var userName := console write:"Enter nickname:"; readLine.
        
        self start name:userName ip:"127.0.0.1" port:DEFAULT_PORT
            | if(:e)[ console writeLine:"Cannot esteblish connection!". AbortException new; raise. ].
            
        console writeLine:"Enter -q to exit the chat".
    ]
    
    prompt
        = ">".
    
    proceed : line
    [
        if(line == "-q")
            [
                self stop.
        
                'program stop.
            ];
            [
                self send text:line.
            ].
            
        ^ emptyLiteralValue.
    ]
}.
