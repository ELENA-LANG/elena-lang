import system'net.
import system'text.
import system'threading.

//limited class TcpClientListener
//{
//    onConnect:client []
//    
//    stacksafe onReceived object:client bytearray:buffer int:length []
//    
//    onError:e for:client []
//}

// --- TcpClient ---

public sealed class TcpClient
{
    T<Thread>    theThread.
    T<Socket>    theSocket.
    T<ByteArray> theBuffer.
    int          theMaxLength.    
    object       theLock.

    Func<TcpClient,object>               event onConnect  :: theConnectFunc.
    Func<TcpClient,ByteArray,int,object> event onReceived :: theReadFunc.
    Func<TcpClient,Exception,object>     event onError    :: theErrorFunc.

    constructor new
        <= new(1024).
    
    constructor new(int size)
    [
        theLock := Object new.
        
        theMaxLength := size.
        theBuffer := ByteArray new(theMaxLength).
    ]

    connect(literal address, int port)
    [
        //resolve server address and port
        T<AddrInfo> addrinfo := AddrInfo new.
        
        // set address info
        addrinfo Family := AF_UNSPEC.
        addrinfo SocketType := SOCK_STREAM.
        addrinfo Protocol := IPPROTO_TCP.
        
        T<AddrInfoReader> reader := AddrInfoReader new(address, port literal, addrinfo).        
        while (reader available)
        [
            reader read(addrinfo).
            
            int       ai_family := addrinfo Family.
            int       ai_socktype := addrinfo SocketType.
            int       ai_protocol := addrinfo Protocol.
            int       ai_addrlen := addrinfo AddressLen.
            dirty_ptr ai_addrptr := addrinfo Address.

            theSocket := Socket new(ai_family, ai_socktype, ai_protocol).

            if (theSocket tryOpen(ai_addrptr, ai_addrlen))
                [
                    reader close.
                    
                    if(nil != theConnectFunc)
                        [ theConnectFunc(self) ].
            
                    ^ self.
                ].
            
            reader next.
        ].
        
        reader close.
        
        TcpClientException new:"Unable to connect to server"; raise.
    ]

    set blocking(bool val)
    [
        int aMode := 0.
        
        ifnot(val)
            [
                aMode := 1.
            ].

        theSocket iocommand(FIONBIO, &aMode).
    ]
    
    set nodelay(bool val)
    [
        theSocket option(IPPROTO_TCP, TCP_NODELAY, val).
    ]
    
    private closeInternal
    [
        theSocket shutdown(SD_BOTH).
        theSocket close.
        
        theSocket := nil.
    ]
    
    close
    [
        lock (theLock)
        [
            if(nil != theSocket)
                [ self closeInternal ]
        ].
    ]
    
    private onError(SocketException e)
    [
        if (nil != theErrorFunc)
            [ theErrorFunc(self, e) ].
        
        int  err := e error_code.
        if(err == WSAECONNRESET)
            [ self closeInternal ]
    ]
    
    private readServer
    [
        int received := 0.
        try(theSocket read(theBuffer, theMaxLength, 0, &received))
        {
            on(SocketException e)
            [
                self onError:e.
            ]
        }.
        
        if (received == 0)
            [ self closeInternal ];
            [ 
                if (received > 0)
                    [ theReadFunc(self, theBuffer, received) ]
            ]
    ]
    
    write(int index, ByteArray buffer, int length)
    [
        int sent := 0.
        theSocket write(buffer, length, 0, &sent).
    ]
    
    write(literal s)
    [
        T<ByteNumber,256> buffer.
        
        int         len := s length.
        int         buf_len := 256.
        int         index := 0.
        while (len > 0)
        [
            if (len < 256)
                [ buf_len := len ];
                [ buf_len := 256 ].
            
            UTF8Encoding convert(index, buf_len, s, T<ByteArray>(buffer)).
            
            self write(0, T<ByteArray>(buffer), buf_len).
            
            len := len - buf_len.
            index := index + buf_len.
        ]
    ]
    
    write(int n)
    [
        T<ByteNumber,4> buffer.
        byteArrayHelper write(T<ByteArray>(buffer), 0, n).
        
        self write(0, T<ByteArray>(buffer), 4)
    ]
    
    private proceed
    [
        bool retVal := false.
        lock (theLock)
        [
            if (nil != theSocket)
                [ self readServer. retVal := true ]
        ].
        
        ^ retVal
    ]
    
    start
    [
        theThread := Thread new:
        [
            while(self proceed)[].
        ].
        
        theThread start.
    ]
}
