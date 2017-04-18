// --- AddrInfo ---

sealed embeddable struct AddrInfo
{
    int       ai_flags.
    int       ai_family.
    int       ai_socktype.
    int       ai_protocol.
    int       ai_addrlen.
    dirty_ptr ai_canonname.
    dirty_ptr ai_addr.
    dirty_ptr ai_next.
    
    stacksafe setFlags &int:aValue
    [
        ai_flags := aValue.
    ]
    
    stacksafe setFamily &int:aValue
    [
        ai_family := aValue.
    ]
    
    stacksafe setSocketType &int:aValue
    [
        ai_socktype := aValue.
    ]
    
    stacksafe setProtocol &int:aValue
    [
        ai_protocol := aValue.
    ]
    
    stacksafe setAddressLen &int:aValue
    [
        ai_addrlen := aValue.
    ]
    
    stacksafe readInfo &vint:aFamily &vint:aSocketType &vint:aProtocol
    [
        aFamily := ai_family.
        aSocketType := ai_socktype.
        aProtocol := ai_protocol.
    ]
    
    stacksafe readAddrPtr &dirty_ptr:ptr &vint:len
    [
        ptr set &dirty_ptr:ai_addr.
        len := ai_addrlen.
    ]

    stacksafe readAddrInfo &literal:aNode &literal:aService &dirty_ptr:addr
    [
        int aRetVal := system'external'WS2_32 getaddrinfo
            &literal:aNode
            &literal:aService
            &addrinfo:$self
            &dirty_ptr:addr.
            
        ifnot (0 == aRetVal)
            [ SocketException new:"getaddrinfo failed" &error_code:(IntNumber new &int:aRetVal) raise. ].
    ]
            
    stacksafe readAddrInfo &literal:aService &dirty_ptr:addr
    [
        int aRetVal := system'external'WS2_32 getaddrinfo
            &int:0
            &literal:aService
            &addrinfo:$self
            &dirty_ptr:addr.
            
        ifnot (0 == aRetVal)
            [ SocketException new:"getaddrinfo failed" &error_code:(IntNumber new &int:aRetVal) raise. ].        
    ]
            
    stacksafe clear
    [
        ai_flags := 0.
        ai_family := 0.
        ai_socktype := 0.
        ai_protocol := 0.
        ai_addrlen := 0.
        ai_canonname set &int:0.
        ai_addr set &int:0.
        ai_next set &int:0.
    ]
}

// --- AddrInfoReader ---

#subject addr_reader = AddrInfoReader.

sealed embeddable struct AddrInfoReader
{
    dirty_ptr paddrinfo.
    dirty_ptr prootinfo.
    
    stacksafe implicit dirty_ptr:addr
    [
        paddrinfo := addr.
        prootinfo := addr.
    ]
    
    stacksafe constructor new &dirty_ptr:addr
    [
        paddrinfo := addr.
        prootinfo := addr.
    ]
    
    bool stacksafe is &available
        = paddrinfo is &empty not.
    
    stacksafe next
    [
        dirty_ptr ptr := paddrinfo.
        ptr offset &int:28.
        
        paddrinfo := ptr.
    ]
    
    stacksafe read &addrinfo:anInfo
    [
        system'external'KERNEL32 RtlMoveMemory
            &addrinfo:anInfo
            &int:paddrinfo
            &int:32. 
    ]
    
    stacksafe close
    [
        system'external'WS2_32 freeaddrinfo
            &int:prootinfo.
    ]
    
    addr_reader = $self.
}

// --- Socket ---

sealed const embeddable struct Socket
{
    __ptr theHandle.
    
    stacksafe constructor $new &int:aSocket
    [
        theHandle := aSocket.
    ]
    
    stacksafe constructor new &socket:aSocket
    [
        theHandle := aSocket.
    ]
    
    stacksafe constructor new &int:anAdressFamily &int:aType &int:aProtocol
        <= $new &int:(system'external'WS2_32 socket &int:anAdressFamily &int:aType &int:aProtocol);
    [
        if (INVALID_SOCKET == theHandle)
            [ SocketException new:"Error at socket" raise. ].
    ]
    
    stacksafe open &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        int aRetVal := system'external'WS2_32 connect 
            &int:theHandle
            &dirty_ptr:aSockAddrPtr
            &int:aNameLen.
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket" raise. ].
    ]

    stacksafe bind &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        int aRetVal := system'external'WS2_32 bind 
            &int:theHandle
            &int:aSockAddrPtr
            &int:aNameLen.
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket" raise. ].
    ]

    stacksafe listen &int:aBackLog
    [
        int aRetVal := system'external'WS2_32 listen 
            &int:theHandle
            &int:aBackLog.
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket" raise. ].
    ]

    stacksafe bool tryOpen &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        int aRetVal := system'external'WS2_32 connect 
            &int:theHandle
            &int:aSockAddrPtr
            &int:aNameLen.
            
        ^ (SOCKET_ERROR != aRetVal).
    ]

    stacksafe write &bytearray:buffer &int:length &int:flags &vint:sent
    [
        int retVal := system'external'WS2_32 send 
            &int:theHandle
            &bytearray:buffer
            &int:length
            &int:flags.
            
        if (SOCKET_ERROR == retVal)
            [ SocketException new:"Error at socket" raise. ].
            
        sent := retVal.            
    ]

    stacksafe read &bytearray:buffer &int:maxLength &int:flags &vint:received
    [
        int retVal := system'external'WS2_32 recv
            &int:theHandle
            &bytearray:buffer
            &int:maxLength
            &int:flags.
        
        if (SOCKET_ERROR == retVal)
        [
            int err := system'external'WS2_32 WSAGetLastError.
            if (err != WSAEWOULDBLOCK)
                [ SocketException new:"Error at socket" raise. ].
        ].
            
        received := retVal.    
    ]

    accept  
    [
        socket aSocket := system'external'WS2_32 accept
            &int:theHandle
            &int:0
            &int:0.
            
        if (INVALID_SOCKET != aSocket)
            [
                ^ Socket new &socket:aSocket.
            ];
            [ ^ $nil. ].
    ]

    stacksafe shutdown &int:how
    [
        int aRetVal := system'external'WS2_32 shutdown
            &int:theHandle
            &int:how.
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket" raise. ].
    ]

    stacksafe iocommand &int:aCommand &vint:aParameter
    [
        int aRetVal := system'external'WS2_32 ioctlsocket
            &int:theHandle
            &int:aCommand
            &vint:aParameter.
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket" raise. ].
    ]

    stacksafe option &int:aLevel &int:anOption &bool:aValue
    [
        int n := aValue iif:1:0.
        
        int aRetVal := system'external'WS2_32 setsockopt
            &int:theHandle
            &int:aLevel
            &int:anOption
            &vint:n
            &int:1.
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket" raise. ].
    ]

    stacksafe close
    [
        system'external'WS2_32 closesocket
            &int:theHandle.
    ]
}