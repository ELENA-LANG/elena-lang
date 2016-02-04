// ==== ver 3.4.1 ===

#import system.
#import system'net.
#import system'threading.

#subject(class:TcpClientListener) tcp_client_listener.
#class(limited) TcpClientListener
{
    #method onConnect:client []
    
    #method(stacksafe)onReceived:client &bytearray:buffer &int:length []
    
    #method onError:e &for:client []
    
    #method tcp_client_listener = $self.
}

// --- TcpClient ---

#class(sealed) TcpClient
{
    #field(type:thread)theListenThread.
    #field theLock.
    
    #field(type:socket)theSocket.
    #field(type:bytearray)theBuffer.
    #field(type:int)theMaxLength.
    
    #field(type:tcp_client_listener)theListener.
    
    #constructor new
        <= new &bufferSize:1024.
    
    #constructor new &bufferSize:size
    [
        theLock := Object new.
        
        theMaxLength := size int.
        theBuffer := ByteArray new &int:theMaxLength.
        
        // winsock start up
        WSAStartUp22.        
    ]

    #method connect &ip:anAddress &port:aPort
    [
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
    
    #method $close
    [
        theSocket shutdown &int:SD_BOTH.
        theSocket close.
        
        theSocket := $nil.
    ]
    
    #method close
    [
        #lock (theLock)
        [
            $self $close.
        ].
    ]
    
    #method tcp_client = $self.
    
    #method set &tcp_client_listener:listener
    [
        theListener := listener.
    ]
    
    #method onError:e
    [
        ($nil != theListener)
            ? [ theListener onError:e &for:$self. ].
        
        #var(type:int) err := e error_code.
        (err == WSAECONNRESET)
            ? [ $self $close. ].
    ]
    
    #method readServer
    [
        #var(type:int)received.
        theSocket read &bytearray:theBuffer &int:theMaxLength &int:0 &vint:received
                | if &SocketError:e
                [
                    $self onError:e.
                ].

        (received == 0)
            ? [ $self $close. ]
            ! [ 
                (received > 0)
                    ? [ theListener onReceived:$self &bytearray:theBuffer &int:received. ].
            ].
    ]
    
    #method(stacksafe) send &int:index &bytearray:buffer &int:length    
    [
        #var(type:int)sent.
        theSocket write &bytearray:buffer &int:length &int:0 &vint:sent.
    ]
    
    #method proceed
    [
        #var retVal := false.
        #lock (theLock)
        [
            ($nil != theSocket)
                ? [ $self readServer. retVal := true. ].
        ].
        
        ^ retVal.
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