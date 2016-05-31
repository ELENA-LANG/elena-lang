#import system.
#import extensions.
#import extensions'dynamic.
#import net.

#class ChatClient :: BaseClient
{
    #field(tcp_client) theNetworkClient.
    
    #field theAddress.
    #field thePort.
    #field theUserName.    
    
    #constructor new &ip:ip &port:port &userName:userName
        <= (new)
    [
        theNetworkClient := TcpClient new.
        
        theAddress := ip.
        thePort := port.
        theUserName := userName literal.
    ]
    
    #method sendNickname
    [
        #var(literal)json := DTO{ Command = INTRODUCTION. Value = theUserName. } toJson literal.
        #var(int)len := json length.
        
        theNetworkClient write &int:len write &literal:json.
    ]
    
    #method send &text:text
    [
        #var(literal)json := DTO{ Command = SEND. Value = text. } toJson literal.
        #var(int)len := json length.
        
        theNetworkClient write &int:len write &literal:json.
    ]
    
    #method start
    [
        theNetworkClient set &tcp_client_listener:TcpClientListener
        {
            onConnect:client
            [
                console writeLine:"connected".
                
                $owner sendNickname.
            ]
            
            onError:e &for:client
            [
                console writeLine:"Exception #":(e error_code).
            ]
            
            onReceived:client &bytearray:buffer &int:length
            [
                $owner write &bytearray:buffer &int:length.
                #var command := $owner readCommand.
                ($nil != command)
                    ? [
                        (command Command) =>
                            TRANSLATE ?
                            [
                                console writeLine writeLine:(command User):":":(command Value) write:">".
                            ].
                    ].                
            ]
        }.
        
        theNetworkClient connect &ip:theAddress &port:thePort.
        
        theNetworkClient set &blocking:false.
        theNetworkClient set &nodelay:true.        
        
        theNetworkClient start.
    ]
    
    #method stop
    [
        theNetworkClient close.
    ]
}