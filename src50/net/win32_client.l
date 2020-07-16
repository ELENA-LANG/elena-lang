import system'net;
import system'text;
import system'threading;
import extensions;

// --- TcpClient ---

public sealed class TcpClient
{
    Thread    theThread;
    Socket    theSocket;
    byte[]    theBuffer;
    int       theMaxLength;    
    object    theLock;

    event Func<TcpClient,object>            onConnect;
    event Func<TcpClient,byte[],int,object> onReceived;
    event Func<TcpClient,Exception,object>  onError;

    constructor new()
        <= new(1024);
    
    constructor new(int size)
    {
        theLock := new Object();
        
        theMaxLength := size;
        theBuffer := new byte[](theMaxLength)
    }

    connect(string address, int port)
    {
        //resolve server address and port
        AddrInfo addrinfo := new AddrInfo();
        
        // set address info
        addrinfo.Family := AF_UNSPEC;
        addrinfo.SocketType := SOCK_STREAM;
        addrinfo.Protocol := IPPROTO_TCP;
        
        AddrInfoReader reader := new AddrInfoReader(address, port.toString(), addrinfo);
        while (reader.Available)
        {
            reader.read(addrinfo);
            
            int       ai_family := addrinfo.Family;
            int       ai_socktype := addrinfo.SocketType;
            int       ai_protocol := addrinfo.Protocol;
            int       ai_addrlen := addrinfo.AddressLen;
            dirty_ptr ai_addrptr := addrinfo.Address;

            theSocket := new Socket(ai_family, ai_socktype, ai_protocol);

            if (theSocket.tryOpen(ai_addrptr, ai_addrlen))
            {
                reader.close();
                
                onConnect.?(self);
        
                ^ self
            };
            
            reader.next()
        };
        
        reader.close();
        
        TcpClientException.new:"Unable to connect to server".raise()
    }

    set Blocking(bool val)
    {
        int mode := val ? 0 : 1;

        theSocket.iocommand(FIONBIO, ref mode)
    }
    
    set Nodelay(bool val)
    {
        theSocket.option(IPPROTO_TCP, TCP_NODELAY, val)
    }
    
    private closeInternal()
    {
        theSocket.shutdown(SD_BOTH);
        theSocket.close();
        
        theSocket := nil
    }
    
    close()
    {
        lock (theLock)
        {
            if(nil != theSocket)
                { self.closeInternal() }
        }
    }
    
    private onError(SocketException e)
    {
        onError.?(self, e);
        
        int  err := e.ErrorCode;
        if(err == WSAECONNRESET)
            { self.closeInternal() }
    }
    
    private readServer()
    {
        int received := 0;
        try
        {
            theSocket.read(theBuffer, theMaxLength, 0, ref received)
        }
        catch(SocketException e)
        {
            self.onError(e)
        };
        
        if (received == 0)
        { 
            self.closeInternal() 
        }
        else
        { 
            if (received > 0)
                { onReceived(self, theBuffer, received) }
        }
    }
    
    write(int index, byte[] buffer, int length)
    {
        int sent := 0;
        theSocket.write(buffer, length, 0, ref sent)
    }
    
    write(string s)
    {
        byte buffer[256];
        
        int         len := s.Length;
        int         buf_len := 256;
        int         index := 0;
        while (len > 0)
        {
            if (len < 256)
            { 
                buf_len := len 
            }
            else
            { 
                buf_len := 256 
            };
            
            UTF8Encoding.convert(index, buf_len, s, buffer);
            
            self.write(0, buffer, buf_len);
            
            len := len - buf_len;
            index := index + buf_len
        }
    }
    
    write(int n)
    {
        byte buffer[4];
        byteArrayHelper.write(buffer, 0, n);
        
        self.write(0, buffer, 4)
    }
    
    private proceed()
    {
        bool retVal := false;
        lock (theLock)
        {
            if (nil != theSocket)
                { self.readServer(); retVal := true }
        };
        
        ^ retVal
    }
    
    start()
    {
        theThread := Thread.assign(
        {
            while(self.proceed()){}
        });
        
        theThread.start()
    }
}
