#import system.
#import system'net.
#import system'text.
#import system'threading.

#subject tcp_client_listener = TcpClientListener.

limited class TcpClientListener
{
    onConnect:client []
    
    stacksafe onReceived:client &bytearray:buffer &int:length []
    
    onError:e &for:client []
}

// --- TcpClient ---

sealed class TcpClient
{
    thread    theListenThread.
    object    theLock.
    
    socket    theSocket.
    bytearray theBuffer.
    int       theMaxLength.
    
    tcp_client_listener theListener.
    
    constructor new
        <= new &bufferSize:1024.
    
    constructor new &bufferSize:size
    [
        theLock := Object new.
        
        theMaxLength := size int.
        theBuffer := ByteArray new &int:theMaxLength.
        
        // winsock start up
        WSAStartUp22.        
    ]

    connect &ip:anAddress &port:aPort
    [
        //resolve server address and port
        addrinfo addrinfo := 0.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_UNSPEC.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        
        dirty_ptr ptr := 0.
        addrinfo readAddrInfo &literal:(anAddress literal) &literal:(aPort literal) &dirty_ptr:ptr.
        
        addr_reader aReader := ptr.
        while (aReader is &available)
        [
            aReader read &addrinfo:addrinfo.
            
            int ai_family := 0.
            int ai_socktype := 0.
            int ai_protocol := 0.
            int ai_addrlen := 0.
            dirty_ptr ai_addrptr := 0.
            addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
            
            theSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
            
            addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
            
            if (theSocket tryOpen &dirty_ptr:ai_addrptr &int:ai_addrlen)
                [
                    aReader close.
                    
                    if($nil != theListener)
                        [ theListener onConnect:$self. ].
            
                    ^ $self.
                ].
            
            aReader next.
        ].
        
        aReader close.
        
        TcpClientException new:"Unable to connect to server" raise.
    ]

    set &blocking:aValue
    [
        int aMode := 0.
        
        ifnot(aValue)
            [
                aMode := 1.
            ].

        theSocket iocommand &int:FIONBIO &vint:aMode.
    ]
    
    set &nodelay:aValue
    [
        theSocket option &int:IPPROTO_TCP &int:TCP_NODELAY &bool:(aValue bool).
    ]
    
    $close
    [
        theSocket shutdown &int:SD_BOTH.
        theSocket close.
        
        theSocket := $nil.
    ]
    
    close
    [
        lock (theLock)
        [
            if($nil != theSocket)
                [ $self $close. ].
        ].
    ]
    
    tcp_client = $self.
    
    set &tcp_client_listener:listener
    [
        theListener := listener.
    ]
    
    onError:e
    [
        if($nil != theListener)
            [ theListener onError:e &for:$self. ].
        
        int  err := e error_code.
        if(err == WSAECONNRESET)
            [ $self $close. ].
    ]
    
    readServer
    [
        int received := 0.
        try(theSocket read &bytearray:theBuffer &int:theMaxLength &int:0 &vint:received)
        {
            SocketError:e
            [
                $self onError:e.
            ]
        }.
        
        if (received == 0)
            [ $self $close. ];
            [ 
                if (received > 0)
                    [ theListener onReceived:$self &bytearray:theBuffer &int:received. ].
            ].
    ]
    
    stacksafe write &int:index &bytearray:buffer &int:length
    [
        int sent := 0.
        theSocket write &bytearray:buffer &int:length &int:0 &vint:sent.
    ]
    
    stacksafe write &literal:s
    [
        byte(256) buffer := 0.
        int       len := s length.
        int       buf_len := 256.
        int       index := 0.
        while (len > 0)
        [
            if (len < 256)
                [ buf_len := len. ];
                [ buf_len := 256. ].
            
            UTF8Encoding convert &int:index &int:buf_len &literal:s &bytearray:buffer.
            
            $self write &int:0 &bytearray:buffer &int:buf_len.
            
            len := len - buf_len.
            index := index + buf_len.
        ].
    ]
    
    stacksafe write &int:n
    [
        byte(4) buffer := 0.
        byteArrayHelper write &bytearray:buffer &int:0 &int:n.
        
        $self write &int:0 &bytearray:buffer &int:4.
    ]
    
    proceed
    [
        var retVal := false.
        lock (theLock)
        [
            if ($nil != theSocket)
                [ $self readServer. retVal := true. ].
        ].
        
        ^ retVal.
    ]
    
    start
    [
        theListenThread := Thread new &func:
        [
            #loop $self proceed.
        ].
        
        theListenThread start.
    ]
}