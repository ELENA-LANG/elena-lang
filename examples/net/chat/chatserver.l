#import system.
#import extensions.
#import net.

#class ChatServer
{
    #field(type:tcp_server)theNetworkServer.
    
    #constructor new &port:port
    [
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
        }.
        
        theNetworkServer start.
    ]
}