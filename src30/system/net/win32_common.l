#define system.

// --- types ---

#subject(class:system'net'Socket)socket.
#subject(class:system'net'WSADATA)wsadata.

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
    ]
}

#static WSAStartUp22 = WSADATA init &byte:2 &byte:2.
