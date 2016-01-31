// ==== ver 3.4.1 ===

#import system.
#import system'net.
#import system'threading.

#subject(class:TcplientListener) tcp_client_listener.
#class(limited) TcpClientListener
{
    #method onConnect:client []
    
    #method(stacksafe)onReceived:client &bytearray:buffer &int:length []
}

// --- TcpClient ---

#class(sealed) TcpClient
{
    #field(type:thread)theListenThread.
    
    #field(type:socket)theSocket.
    #field(type:bytearray)theBuffer.
    #field(type:int)theMaxLength.
    
    #field(type:tcp_client_listener)theListener.
    
    #constructor new &ip:address &port:port
        <= new &ip:address &port:port &bufferSize:1024.
    
    #constructor new &ip:anAddress &port:aPort &bufferSize:size
    [
        theMaxLength := size int.
        theBuffer := ByteArray new &int:theMaxLength.
        
        // winsock start up
        WSAStartUp22.
        
        //resolve server address and port
        #var(type:addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_UNSPEC.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        
        #var(type:addr_reader)aReader := AddrInfoReader new &literal:(anAddress literal) &literal:(aPort literal) &addrinfo:addrinfo.
        #loop (aReader is &available)?
        [
            aReader read &addrinfo:addrinfo.
            
            #var(type:int)ai_family.
            #var(type:int)ai_socktype.
            #var(type:int)ai_protocol.
            #var(type:int)ai_addrlen.
            #var(type:dirty_ptr)ai_addrptr.
            addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
            
            theSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
            
            addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
            
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
    
    #method set &tcp_client_listener:listener
    [
        theListener := listener.
    ]
    
    #method readServer
    [
        #var(type:int)received.
        theSocket read &bytearray:theBuffer &int:theMaxLength &int:0 &vint:received.
        (received == 0)
            ? [ $self close. ]
            ! [ theListener onReceived:$self &bytearray:theBuffer &int:received. ].
    ]
    
    #method(stacksafe) send &int:index &bytearray:buffer &int:length    
    [
        #var(type:int)sent.
        theSocket write &bytearray:buffer &int:length &int:0 &vint:sent.
    ]
    
    #method proceed
    [
        $self readServer.
        
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