//public const int INVALID_SOCKET = -1.
//public const int SOCKET_ERROR   = -1.
//
//public const int AI_PASSIVE  = 1.
//public const int AF_UNSPEC   = 0.
//public const int AF_INET     = 2.
//public const int SOCK_STREAM = 1.
//public const int IPPROTO_TCP = 6.
//
//public const int TCP_NODELAY = 1. 
//
//public const int FIONBIO = 8004667Eh.
//
//public const int WSAEWOULDBLOCK = 10035.
//public const int WSAECONNRESET  = 10054.
//
//public const int SOMAXCONN = 5.
//
//public const int SD_BOTH = 2.
//
//// --- WSAException ---
//
//public class SocketException :: Exception
//{
//    int theCode.
//    
//    /// Creates the exception
//    constructor new(literal message) error_code(int code)
//        <= new(message);
//    [
//        theCode := code.
//    ]
//    
//    constructor new(literal message)
//        <= new(message);
//    [
//        theCode := system'external'WS2_32 WSAGetLastError.
//    ]
//    
//    int error_code = theCode.
//    
//    /// Returns the message and the call stack
//    T<literal> literal
//    [
//        literal retVal := theMessage add($10"Socket Error code:"); add(theCode literal); add($10"Call stack:"$10).
//        
//        ^ retVal add(theCallStack literal)
//    ]
//}

// --- WSAStartUp ---

sealed struct WSADATA
{
    short wVersion;
    short wHighVersion;
    byte  szDescription[257];
    byte  szSystemStatus[129];
    short iMaxSockets;
    short iMaxUdpDg;
    int   lpVendorInfo;
    
    constructor init(int version, int highVersion)
        <= init(cast byte(version), cast byte(highVersion));
    
    constructor init(byte version, byte highVersion)
    {
        short wVersionRequested := version * 10h + highVersion;

//        int aRetVal := system'external'WS2_32 WSAStartup(wVersionRequested, self).
//            
//        ifnot (0 == aRetVal)
//            [ SocketException new:"Cannot start socket service" error_code(aRetVal); raise ].
//            
//        startUpEvents onStop := (:e)[ self close ].
    }
    
//    close
//    [
//        system'external'WS2_32 WSACleanup.
//    ]
}

//preloaded static WSAStartUp22 = WSADATA init(2, 2).
