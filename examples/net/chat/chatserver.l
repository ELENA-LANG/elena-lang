#import system.
#import system'io.
#import extensions.
#import net.

#class ChatServer
{
    #field(type:tcp_server)theNetworkServer.
    
    #field(type:membuffer)theBuffer.
    
    #constructor new &port:port
    [
        theBuffer := MemoryBuffer new.
        
        theNetworkServer := TcpServer new &port:port.
        
        theNetworkServer set &blocking:false.
    ]
    
    #method start
    [
        theNetworkServer set &tcp_server_listener:TcpServerListener
        {
            onConnect:client
            [
                console writeLine:"new client joint".
            ]
            
            onLeave:client
            [
                console writeLine:"client left".
            ]
            
            onError:e &for:client
            [
                console writeLine:"exception #":(e error_code).
            ]
            
            onReceived:client &bytearray:buffer &int:length 
            [
                theBuffer write &int:0 &int:length &bytearray:buffer.
            ]
        }.
        
        theNetworkServer start.
    ]
}