#import system.
#import net.

#class ChatClient
{
    #field(type:tcp_client)theNetworkClient.
    
    #constructor new &ip:ip &port:port
    [
        theNetworkClient := TcpClient new &ip:ip &port:port.
        
        theNetworkClient set &blocking:false.
        theNetworkClient set &nodelay:true.
    ]
}