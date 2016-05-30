#import system.
#import system'net.
#import system'text.
#import system'threading.

#subject tcp_client_listener = TcpClientListener.
#class(limited) TcpClientListener
{
    #method onConnect:client []
    
    #method(stacksafe)onReceived:client &bytearray:buffer &int:length []
    
    #method onError:e &for:client []
}

// --- TcpClient ---

#class(sealed) TcpClient
{
    #field(thread)theListenThread.
    #field theLock.
    
    #field(socket)theSocket.
    #field(bytearray)theBuffer.
    #field(int)theMaxLength.
    
    #field(tcp_client_listener)theListener.
    
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
        #var(addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_UNSPEC.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        
        #var(dirty_ptr)ptr.
        addrinfo readAddrInfo &literal:(anAddress literal) &literal:(aPort literal) &dirty_ptr:ptr.
        
        #var(addr_reader)aReader := ptr.
        #loop (aReader is &available)?
        [
            aReader read &addrinfo:addrinfo.
            
            #var(int)ai_family.
            #var(int)ai_socktype.
            #var(int)ai_protocol.
            #var(int)ai_addrlen.
            #var(dirty_ptr)ai_addrptr.
            addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
            
            theSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
            
            addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
            
            (theSocket tryOpen &dirty_ptr:ai_addrptr &int:ai_addrlen)
                ? [
                    aReader close.
                    
                    ($nil != theListener)
                        ? [ theListener onConnect:$self. ].
            
                    ^ $self.
                ].
            
            aReader next.
        ].
        
        aReader close.
        
        #throw TcpClientException new:"Unable to connect to server".
    ]

    #method set &blocking:aValue
    [
        #var(int)aMode.
        
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
            ($nil != theSocket)
                ? [ $self $close. ].
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
        
        #var(int) err := e error_code.
        (err == WSAECONNRESET)
            ? [ $self $close. ].
    ]
    
    #method readServer
    [
        #var(int)received.
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
    
    #method(stacksafe) write &int:index &bytearray:buffer &int:length
    [
        #var(int)sent.
        theSocket write &bytearray:buffer &int:length &int:0 &vint:sent.
    ]
    
    #method(stacksafe) write &literal:s
    [
        #var(byte:256)buffer.
        #var(int)len := s length.
        #var(int)buf_len := 256.
        #var(int)index := 0.
        #loop (len > 0)?
        [
            (len < 256)
                ? [ buf_len := len. ]
                ! [ buf_len := 256. ].
            
            UTF8Encoding convert &int:index &int:buf_len &literal:s &bytearray:buffer.
            
            $self write &int:0 &bytearray:buffer &int:buf_len.
            
            len := len - buf_len.
            index := index + buf_len.
        ].
    ]
    
    #method(stacksafe) write &int:n
    [
        #var(byte:4)buffer.
        buffer write &int:0 &int:n.
        
        $self write &int:0 &bytearray:buffer &int:4.
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