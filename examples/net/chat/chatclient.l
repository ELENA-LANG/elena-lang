#import system.
#import extensions.
#import net.

#class ChatClient
{
    #field(type:tcp_client)theNetworkClient.
    #field theAddress.
    #field thePort.
    
    #constructor new &ip:ip &port:port
    [
        theNetworkClient := TcpClient new.
        
        theAddress := ip.
        thePort := port.
    ]
    
    #method start
    [
        theNetworkClient connect &ip:theAddress &port:thePort.
        
        theNetworkClient set &blocking:false.
        theNetworkClient set &nodelay:true.
        
        theNetworkClient set &tcp_client_listener:TcpClientListener
        {
            onConnect:client
            [
                console writeLine:"new client".
            ]
            
            onError:e &for:client
            [
                console writeLine:"Exception #":(e error_code).
            ]
        }.
        
        theNetworkClient start.
    ]
    
    #method stop
    [
        theNetworkClient close.
    ]
}