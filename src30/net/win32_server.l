// ==== ver 3.4.1 ===

#import system.
#import system'collections.
#import system'net.
#import system'threading.

#class(sealed)TcpRemoteClient
{
    #field(type:socket)theSocket.
    
    #constructor new &socket:socket
    [
        theSocket := socket.
    ]
    
    #method tcp_remoteclient = $self.
}

#class(sealed)TcpServer
{
    #field(type:socket)theListenSocket.
    #field(type:array_list)theClients.
    #field(type:thread)theListenThread.
    #field(type:func1)theNewClient.
    
    #constructor new &port:aPort
    [
        theClients := ArrayList new.
        
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
        #var(type:addr_reader)aReader := AddrInfoReader new &literal:(aPort literal) &addrinfo:addrinfo.        
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
    
    #method set &onNewClient:aFunction
    [
        theNewClient := aFunction func1.
    ]
    
    #method newClient
    [
        #var socket := theListenSocket accept.
        #var client := $nil.
        ($nil != socket)?
        [
            socket option &int:IPPROTO_TCP &int:TCP_NODELAY &bool:true.
            
            client := TcpRemoteClient new &socket:socket.
            
            theClients += client.
        ].
        
        ^ client.
    ]
    
    #method proceed
    [
        #var client := $self newClient.
        ($nil != client)?
        [
            ($nil != theNewClient)
                ? [ theNewClient eval:client. ].
        ].
        
        ^ true.
    ]
    
    #method start
    [
        theListenThread := Thread new &func:
        [
            #loop $self proceed.
        ].
        
        theListenThread start.
    ]
}