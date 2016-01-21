// ==== ver 3.4.1 ===

#import system.
#import system'net.

#class(sealed)TcpServer
{
    #field(type:socket)theListenSocket.
    
    #constructor new &port:aPort
    [
        // winsock start up
        WSAStartUp22.
        
        //resolve server address and port
        #var(type:addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_INET.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        addrinfo setFlags &int:AI_PASSIVE.
        
        // Create a SOCKET for connecting to server
        #var(type:addr_reader)aReader := AddrInfoReader new &wide:(aPort wide).        
        aReader read &addrinfo:addrinfo.
        
        #var(type:int)ai_family.
        #var(type:int)ai_socktype.
        #var(type:int)ai_protocol.
        #var(type:int)ai_addrlen.
        #var(type:dirty_ptr)ai_addrptr.
        addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
        
        theListenSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
        
        addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
        
        theListenSocket bind &dirty_ptr:ai_addrptr &int:ai_addrlen.
        
        aReader close.
        
        theListenSocket listen &int:SOMAXCONN.
    ]
    
    #method set &blocking:aValue
    [
        #var(type:int)aMode.
        
        aValue 
            ? [
                aMode := 0.
            ]
            ! [
                aMode := 1.
            ].

        theListenSocket iocommand &int:FIONBIO &vint:aMode.
    ]    
    
    #method tcp_server = $self.
}