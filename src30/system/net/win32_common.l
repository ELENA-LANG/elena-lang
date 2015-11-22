// ==== ver 3.4.1 ===

#import system.

#symbol(type:int,const) INVALID_SOCKET = -1.
#symbol(type:int,const) SOCKET_ERROR   = -1.

#symbol(type:int,const) AI_PASSIVE  = 1.
#symbol(type:int,const) AF_UNSPEC   = 0.
#symbol(type:int,const) AF_INET     = 2.
#symbol(type:int,const) SOCK_STREAM = 1.
#symbol(type:int,const) IPPROTO_TCP = 6.

#symbol(type:int,const) TCP_NODELAY = 1. 

#symbol(type:int,const) FIONBIO = 8004667Eh.

#symbol(type:int,const) SOMAXCONN = 5.

// --- types ---

#subject(class:system'net'Socket)  socket.
#subject(class:system'net'WSADATA) wsadata.
#subject(class:AddrInfo)           addrinfo.

// --- WSAException ---

#class SocketException :: Exception
{
    #field theCode.
    
    /// Creates the exception
    #constructor new:aMessage &error_code:aCode
        <= (%new &literal:(aMessage literal) &subject: %SocketError)
    [
        theCode := aCode.
    ]
    
    #constructor(stacksafe) new:aMessage
        <= (%new &literal:(aMessage literal) &subject: %SocketError)
    [
        theCode := IntNumber new &int:(system'external'WS2_32 WSAGetLastError).
    ]
}

// --- WSAStartUp ---

#class(sealed,struct) WSADATA
{
    #field(type:short) wVersion.
    #field(type:short) wHighVersion.
    #field(type:bytearray,size:257) szDescription.
    #field(type:bytearray,size:129) szSystemStatus.
    #field(type:short) iMaxSockets.
    #field(type:short) iMaxUdpDg.
    #field(type:int)   lpVendorInfo.
    
    #constructor init &byte:aVersion &byte:aHighVersion
    [
        #var(type:short)wVersionRequested := aVersion * 10h + aHighVersion.

        #var(type:int)aRetVal :=  system'external'WS2_32 WSAStartup 
            &short:wVersionRequested
            &wsadata:$self.
            
        (0 == aRetVal)
            ! [ #throw SocketException new:"Cannot start socket service" &error_code:(IntNumber new &int:aRetVal). ].
            
        'startUp set &onAppEnd: arg [ $self close ].
    ]
    
    #method wsadata = $self.
    
    #method close
    [
        system'external'WS2_32 WSACleanup.
    ]
}

#static WSAStartUp22 = WSADATA init &byte:2 &byte:2.
