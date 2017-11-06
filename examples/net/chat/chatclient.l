import extensions.
import extensions'dynamic.
import net.

class ChatClient :: BaseClient
{
    tcp_client theNetworkClient.
    
    object     theAddress.
    object     thePort.
    object     theUserName.    
    
    constructor new ip:ip port:port userName:userName
        <= new;
    [
        theNetworkClient := TcpClient new.
        
        theAddress := ip.
        thePort := port.
        theUserName := userName literal.
    ]
    
    sendNickname
    [
        literal json := CommandDTO::{ @_command := INTRODUCTION. @_value := theUserName. } toJson; literal.
        int len := json length.
        
        theNetworkClient write int:len; write literal:json.
    ]
    
    send text:text
    [
        literal json := CommandDTO::{ @_command := SEND. @_value := text. } toJson; literal.
        int len := json length.
        
        theNetworkClient write int:len; write literal:json.
    ]
    
    start
    [
        theNetworkClient set tcp_client_listener(TcpClientListener::
        {
            onConnect:client
            [
                console writeLine:"connected".
                
                $self sendNickname.
            ]
            
            onError:e for:client
            [
                console printLine("Exception #",e error_code).
            ]
            
            onReceived object:client bytearray:buffer int:length
            [
                $self write bytearray:buffer int:length.
                var command := $self readCommand.
                if ($nil != command)
                    [
                        (command Command) =>
                            TRANSLATE
                            [
                                console printLine; printLine(command User,":",command Value); print:">".
                            ].
                    ].                
            ]
        }).
        
        theNetworkClient connect ip:theAddress port:thePort.
        
        theNetworkClient blocking := false.
        theNetworkClient nodelay := true.        
        
        theNetworkClient start.
    ]
    
    method stop
    [
        theNetworkClient close.
    ]
}