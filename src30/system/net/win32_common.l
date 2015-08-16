#define system.

// --- types ---

#subject(class:system'net'Socket)socket.

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
    
    #constructor init &short:aVersion &short:aHighVersion
    [
    ]
}

#static WSAStartUp22 = WSADATA init &short:2 &short:2.
