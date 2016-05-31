#import system.
#import system'dynamic.

#symbol(const,int)DEFAULT_PORT = 6881.

#class Client
{
    #field theClient.
    
    #method start &name:userName &ip:ip &port:port
    [
        theClient := ChatClient new &ip:ip &port:port &userName:userName.
        
        theClient start.
    ]
    
    #method send &text:text
        => theClient.
    
    #method stop
    [
        theClient stop.
    ]
}

#symbol control = Client new mix &into:
{
    greeting
    [
        console writeLine:"ELENA Char Client".
        
        #var userName := console write:"Enter nickname:" readLine.
        
        this start &name:userName &ip:"127.0.0.1" &port:DEFAULT_PORT
            | if &Error: e [ console writeLine:"Cannot esteblish connection!". #throw AbortException new. ].
            
        console writeLine:"Enter -q to exit the chat".
    ]
    
    prompt
        = ">".
    
    proceed : line
    [
        (line == "-q")
            ? [
                this stop.
        
                'program stop.
            ]
            ! [
                this send &text:line.
            ].
            
        ^ emptyLiteralValue.
    ]
}.
