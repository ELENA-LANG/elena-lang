//import extensions.
//import extensions'dynamic.
import net.

class ChatClient :: BaseClient
{
    T<TcpClient> theNetworkClient.
    
    literal      theAddress.
    int          thePort.
    literal      theUserName.    
    
    constructor new(literal ip, int port, literal userName)
        <= new;
    [
        theNetworkClient := TcpClient new.
        
        theAddress := ip.
        thePort := port.
        theUserName := userName.
    ]
    
    sendNickname
    [
        literal json := CommandDTO::{ @_command := INTRODUCTION. @_value := theUserName. } toJson; literal.
        int len := json length.
        
        theNetworkClient write(len); write(json).
    ]
    
    send(literal text)
    [
        literal json := CommandDTO::{ @_command := SEND. @_value := text. @_user := nil. } toJson; literal.
        int len := json length.
        
        theNetworkClient write(len); write(json).
    ]
    
    start
    [
        theNetworkClient onConnect := (:client)<TcpClient>
        [            
            console writeLine:"connected".
            
            self sendNickname.
        ].
        
        theNetworkClient onError := (:client:e)<TcpClient,Exception>
        [
            console printLine("Exception #",e error_code).
        ].
        
        theNetworkClient onReceived := (:client:buffer:length)<TcpClient,ByteArray,int>
        [
            self write(buffer, length).
            var command := self readCommand.
            if (nil != command)
                [
                    (command Command) =>
                        TRANSLATE
                        [
                            console printLine; printLine(command User,":",command Value); print:">".
                        ].
                ].                
        ].
        
        theNetworkClient connect(theAddress, thePort).
        
        theNetworkClient blocking := false.
        theNetworkClient nodelay := true.        
        
        theNetworkClient start.
    ]
    
    stop
    [
        theNetworkClient close.
    ]
}