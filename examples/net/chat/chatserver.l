#import system.
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
                console writeLine:"new client".
            ]
        }.
        
        theNetworkServer start.
    ]
}