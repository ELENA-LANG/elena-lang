#import system.
#import system'net.

// --- TcpClient ---

#class(sealed) TcpClient
{
    #field(type:socket)theSocket.
    
    #constructor new &ip:anAddress &port:aPort
    [
        // winsock start up
        WSAStartUp22.
        
        //resolve server address and port
        #var(type:addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_UNSPEC.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        
        #var(type:addr_reader)aReader := AddrInfoReader new &wide:(anAddress wide) &wide:(aPort wide).
        #loop (aReader is &available)?
        [
            aReader read &addrinfo:addrinfo.
            
            #var(type:int)ai_family.
            #var(type:int)ai_socktype.
            #var(type:int)ai_protocol.
            #var(type:int)ai_addrlen.
            #var(type:dirty_ptr)ai_addrptr.
            addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol &vint:ai_addrlen.
            
            theSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
            
            addrinfo readAddrPtr &dirty_ptr:ai_addrptr.
            
            (theSocket tryOpen &dirty_ptr:ai_addrptr &int:ai_addrlen)
                ? [
                    aReader close.
                    
                    ^ $self.
                ].
            
            aReader next.
        ].
        
        aReader close.
        
        #throw TcpClientException new:"Unable to connect to server".
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

        theSocket iocommand &int:FIONBIO &vint:aMode.
    ]
    
    #method set &nodelay:aValue
    [
        theSocket option &int:IPPROTO_TCP &int:TCP_NODELAY &bool:(aValue bool).
    ]
    
    #method close
    [
        theSocket close.
    ]
    
    #method tcp_client = $self.
}