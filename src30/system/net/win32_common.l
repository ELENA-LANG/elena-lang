#import system.

#symbol(int,const) INVALID_SOCKET = -1.
#symbol(int,const) SOCKET_ERROR   = -1.

#symbol(int,const) AI_PASSIVE  = 1.
#symbol(int,const) AF_UNSPEC   = 0.
#symbol(int,const) AF_INET     = 2.
#symbol(int,const) SOCK_STREAM = 1.
#symbol(int,const) IPPROTO_TCP = 6.

#symbol(int,const) TCP_NODELAY = 1. 

#symbol(int,const) FIONBIO = 8004667Eh.

#symbol(int,const) WSAEWOULDBLOCK = 10035.
#symbol(int,const) WSAECONNRESET  = 10054.

#symbol(int,const) SOMAXCONN = 5.

#symbol(int,const) SD_BOTH = 2.

// --- types ---

#subject socket   = system'net'Socket.
#subject wsadata  = system'net'WSADATA.
#subject addrinfo = system'net'AddrInfo.

// --- WSAException ---

#class(exceptionof:SocketError) SocketException
{
    #field theCode.
    
    /// Creates the exception
    #constructor new:message &error_code:aCode
    [
        theMessage := message literal.
        theCallStack := CallStack new.
        theCode := aCode.
    ]
    
    #constructor(stacksafe) new:message
    [
        theMessage := message literal.
        theCallStack := CallStack new.
        theCode := IntNumber new &int:(system'external'WS2_32 WSAGetLastError).
    ]
    
    #method(int) error_code = theCode int.
    
    /// Returns the message and the call stack
    #method literal = theMessage add &literal:#10"Socket Error code:" add &literal:(theCode literal) add &literal:#10"Call stack:"#10 add &literal:(theCallStack literal).
}

// --- WSAStartUp ---

#class(sealed,embeddable) WSADATA
{
    #field(short)    wVersion.
    #field(short)    wHighVersion.
    #field(byte:257) szDescription.
    #field(byte:129) szSystemStatus.
    #field(short)    iMaxSockets.
    #field(short)    iMaxUdpDg.
    #field(int)      lpVendorInfo.
    
    #constructor(stacksafe) init &byte:aVersion &byte:aHighVersion
    [
        #var(short)wVersionRequested := aVersion * 10h + aHighVersion.

        #var(int)aRetVal :=  system'external'WS2_32 WSAStartup 
            &short:wVersionRequested
            &wsadata:$self.
            
        (0 == aRetVal)
            ! [ #throw SocketException new:"Cannot start socket service" &error_code:(IntNumber new &int:aRetVal). ].
            
        startUpEvents set &onAppEnd: arg [ $self close ].
    ]
    
    #method wsadata = $self.
    
    #method close
    [
        system'external'WS2_32 WSACleanup.
    ]
}

#static WSAStartUp22 = WSADATA init &byte:2 &byte:2.
