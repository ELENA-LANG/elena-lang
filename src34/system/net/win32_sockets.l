//// --- IN_ADDR ---
//
//sealed struct IN_ADDR
//{
//    long addr.    
//    
//    stacksafe set long(LongNumber addr)
//    [
//        @addr := addr.
//    ]
//    
//    stacksafe clear
//    [
//        addr := 0l.
//    ]
//}
//
//// --- SOCKADDR_IN ---
//
//sealed struct SOCKADDR_IN
//{
//    short      prop family :: sin_family.
//    short      prop port   :: sin_port.
//    T<IN_ADDR> sin_addr.
//    V<byte,4>  sin_zero. 
//
//    set family(IntNumber n)
//    [
//        short n16 := n.
//        
//        self family := n16.
//    ]
//
//    set port(IntNumber n)
//    [
//        short n16 := n.
//        
//        self port := n16.
//    ]
//
//    set ip_address(LiteralValue ip)
//    [
//        long addr := system'external'WS2_32 inet_addr(ip).
//        
//        sin_addr long := addr.
//    ]
//
//    embeddable constructor new
//    [
//        sin_family := 0.
//        sin_port := 0.
//        
//        sin_zero[0]:=0.
//        sin_zero[1]:=0.
//        sin_zero[2]:=0.
//        sin_zero[3]:=0.
//
//        sin_addr clear.
//    ]
//}
//
////const int SOCKADDR_IN_SIZE = 16.

// --- AddrInfo ---

public sealed struct AddrInfo
{
    int       ai_flags.
    int       ai_family.
    int       ai_socktype.
    int       ai_protocol.
    int       ai_addrlen.
    dirty_ptr ai_canonname.
    dirty_ptr ai_addr.
    dirty_ptr ai_next.
    
    embeddable stacksafe constructor new
    [
        self clear.
    ]
    
    int rvprop Flags :: readFlagsTo.
    
    stacksafe set Flags(int val)
    [
        ai_flags := val.
    ]
    
    stacksafe readFlagsTo(ref<int> retVal)
    [
        retVal value := ai_flags.
    ]
    
    int rvprop Family :: readFamilyTo.
    
    stacksafe set Family(int val)
    [
        ai_family := val.
    ]

    stacksafe readFamilyTo(ref<int> retVal)
    [
        retVal value := ai_family.
    ]
        
    int rvprop SocketType :: readSocketTypeTo.        
        
    stacksafe set SocketType(int val)
    [
        ai_socktype := val.
    ]
    
    stacksafe readSocketTypeTo(ref<int> retVal)
    [
        retVal value := ai_socktype.
    ]
    
    int rvprop Protocol :: readProtocolTo.        
    
    stacksafe set Protocol(int val)
    [
        ai_protocol := val.
    ]
    
    stacksafe readProtocolTo(ref<int> retVal)
    [
        retVal value := ai_protocol.
    ]
    
    int rvprop AddressLen :: readAddressLenTo.
    
    stacksafe set AddressLen(int val)
    [
        ai_addrlen := val.
    ]

    stacksafe readAddressLenTo(ref<int> retVal)
    [
        retVal value := ai_addrlen.
    ]
        
    dirty_ptr rvprop Address :: readAddressTo.

    stacksafe readAddressTo(ref<dirty_ptr> retVal)
    [
        retVal value := ai_addr.
    ]
                
    stacksafe clear
    [
        ai_flags := 0.
        ai_family := 0.
        ai_socktype := 0.
        ai_protocol := 0.
        ai_addrlen := 0.
        ai_canonname := T<dirty_ptr>().
        ai_addr := T<dirty_ptr>().
        ai_next := T<dirty_ptr>().
    ]
}

// --- AddrInfoReader ---

public sealed struct AddrInfoReader
{
    dirty_ptr paddrinfo.
    dirty_ptr prootinfo.
    
    stacksafe /*embeddable*/ constructor new(literal service, AddrInfo info)
    [
        dirty_ptr ptr := T<dirty_ptr>().
        int retVal := system'external'WS2_32 getaddrinfo(0, service, info, ptr).
            
        ifnot (0 == retVal)
            [ SocketException new:"getaddrinfo failed" error_code(retVal); raise ].
            
        paddrinfo := ptr.
        prootinfo := ptr.
    ]

    stacksafe /*embeddable*/ constructor new(literal node, literal service, AddrInfo info)
    [
        dirty_ptr ptr := T<dirty_ptr>().
        int retVal := system'external'WS2_32 getaddrinfo(node, service, info, ptr).
            
        ifnot (0 == retVal)
            [ SocketException new:"getaddrinfo failed" error_code(retVal); raise ].
            
        paddrinfo := ptr.
        prootinfo := ptr.
    ]

    bool stacksafe available
        = paddrinfo isEmpty; inverted.
    
    stacksafe next
    [
        dirty_ptr ptr := paddrinfo.
        ptr := ptr + 28.
        
        paddrinfo := ptr.
    ]
    
    stacksafe read(AddrInfo info)
    [
        system'external'KERNEL32 RtlMoveMemory(info, paddrinfo, 32).
    ]
    
    stacksafe close
    [
        system'external'WS2_32 freeaddrinfo(prootinfo).
    ]
}

// --- Socket ---

public sealed const struct Socket
{
    int theHandle.
    
    constructor(int handle)
    [
        theHandle := handle.
    ]
    
    stacksafe constructor new(Socket socket)
    [
        theHandle := T<int>(socket).
    ]
    
    cast T<int> = theHandle.
    
    stacksafe constructor new(int adressFamily, int type, int protocol)
    [
        theHandle := system'external'WS2_32 socket(adressFamily, type, protocol).
        
        if (INVALID_SOCKET == theHandle)
            [ SocketException new:"Error at socket"; raise. ].
    ]
    
//    stacksafe open dirty_ptr:aSockAddrPtr int:aNameLen
//    [
//        int aRetVal := system'external'WS2_32 connect 
//            int:theHandle
//            dirty_ptr:aSockAddrPtr
//            int:aNameLen.
//            
//        if (SOCKET_ERROR == aRetVal)
//            [ SocketException new:"Error at socket"; raise. ].
//    ]

    stacksafe bind(dirty_ptr sockAddrPtr, int nameLen)
    [
        int retVal := system'external'WS2_32 bind(theHandle, sockAddrPtr, nameLen).
            
        if (SOCKET_ERROR == retVal)
            [ SocketException new:"Error at socket"; raise ].
    ]

    stacksafe listen(int backLog)
    [
        int aRetVal := system'external'WS2_32 listen(theHandle, backLog).
            
        if (SOCKET_ERROR == aRetVal)
            [ SocketException new:"Error at socket"; raise ].
    ]

//    stacksafe bool tryOpen dirty_ptr:aSockAddrPtr int:aNameLen
//    [
//        int aRetVal := system'external'WS2_32 connect 
//            int:theHandle
//            int:aSockAddrPtr
//            int:aNameLen.
//            
//        ^ (SOCKET_ERROR != aRetVal).
//    ]
//
//    stacksafe connect(literal ip_address, int port)
//    [
//        type<SOCKADDR_IN> addr := SOCKADDR_IN new.
//        addr port := port.
//        addr family := system'net'AF_INET.
//        addr ip_address := ip_address.
//        
//        dirty_ptr ptr := addr.        
//        int retVal := system'external'WS2_32 connect 
//            int:theHandle
//            int:ptr
//            int:SOCKADDR_IN_SIZE.
//            
//        if (SOCKET_ERROR == retVal)
//            [ SocketException new:"Error at socket"; raise ].
//    ]
//
//    stacksafe write bytearray:buffer
//    [
//        int len := buffer length.
//        int sent := 0.
//        $self write bytearray:buffer int:len int:0 vint:sent.        
//        
//        ^ sent.
//    ]
//
//    stacksafe write bytearray:buffer int:length int:flags vint:sent
//    [
//        int retVal := system'external'WS2_32 send 
//            int:theHandle
//            bytearray:buffer
//            int:length
//            int:flags.
//            
//        if (SOCKET_ERROR == retVal)
//            [ SocketException new:"Error at socket"; raise ].
//            
//        sent int := retVal.            
//    ]
//
//    stacksafe read bytearray:buffer int:maxLength int:flags vint:received
//    [
//        int retVal := system'external'WS2_32 recv
//            int:theHandle
//            bytearray:buffer
//            int:maxLength
//            int:flags.
//        
//        if (SOCKET_ERROR == retVal)
//        [
//            int err := system'external'WS2_32 WSAGetLastError.
//            if (err != WSAEWOULDBLOCK)
//                [ SocketException new:"Error at socket"; raise ].
//        ].
//            
//        received int := retVal.    
//    ]

    T<Socket> accept  
    [
        T<Socket> socket := system'external'WS2_32 accept(theHandle, 0, 0).
            
        if (INVALID_SOCKET != socket)
            [
                ^ socket
            ];
            [ ^ nil ]
    ]

//    stacksafe shutdown int:how
//    [
//        int aRetVal := system'external'WS2_32 shutdown
//            int:theHandle
//            int:how.
//            
//        if (SOCKET_ERROR == aRetVal)
//            [ SocketException new:"Error at socket"; raise. ].
//    ]

    stacksafe iocommand(int command, ref<int> parameter)
    [
        int retVal := system'external'WS2_32 ioctlsocket(theHandle, command, parameter).
            
        if (SOCKET_ERROR == retVal)
            [ SocketException new:"Error at socket"; raise ].
    ]

//    stacksafe option int:aLevel int:anOption bool:aValue
//    [
//        int n := aValue iif(1,0).
//        
//        int aRetVal := system'external'WS2_32 setsockopt
//            int:theHandle
//            int:aLevel
//            int:anOption
//            vint:n
//            int:1.
//            
//        if (SOCKET_ERROR == aRetVal)
//            [ SocketException new:"Error at socket"; raise ]
//    ]
//
//    stacksafe close
//    [
//        system'external'WS2_32 closesocket
//            int:theHandle
//    ]
}