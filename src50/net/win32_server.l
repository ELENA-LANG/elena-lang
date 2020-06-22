import system'collections;
import system'net;
import system'threading;
import system'text;

public sealed class TcpRemoteClient
{
    Socket theSocket;
    
    prop object Parameter;
    
    constructor assign(Socket socket)
    {
        theSocket := socket
    }
    
    read(byte[] buffer, int maxLength, ref int recv)
    {
        theSocket.read(buffer, maxLength, 0, ref recv)
    }
    
    write(byte[] buffer, int length)
    {
        int sent := 0;
        theSocket.write(buffer, length, 0, ref sent);
    }
    
    close()
        => theSocket;
}

public sealed class TcpServer
{
    Socket                theListenSocket;
    List<TcpRemoteClient> theClients;
    Thread                theListenThread;
    byte[]                theBuffer;
    int                   theMaxLength;

    event Func<TcpRemoteClient,object> OnConnect;
    event Func<TcpRemoteClient,object> OnLeave;
    event Func<TcpRemoteClient,byte[],int,object> OnReceived;
    event Func<TcpRemoteClient,SocketException,object> OnError;
    
    constructor assign(int port)
        <= assign(port, 1024);
    
    constructor assign(int port, int bufferSize)
    {
        theMaxLength := bufferSize;
        theBuffer := new byte[](theMaxLength);

        theClients := new List<TcpRemoteClient>();
        
        //resolve server address and port
        AddrInfo addrinfo := new AddrInfo();
        
        // set address info
        addrinfo.Family := AF_INET;
        addrinfo.SocketType := SOCK_STREAM;
        addrinfo.Protocol := IPPROTO_TCP;
        addrinfo.Flags  := AI_PASSIVE;
        
        // Create a SOCKET for connecting to server
        AddrInfoReader reader := new AddrInfoReader(port.toPrintable(), addrinfo);
        
        reader.read(addrinfo);
        int       ai_family := addrinfo.Family;
        int       ai_socktype := addrinfo.SocketType;
        int       ai_protocol := addrinfo.Protocol;
        int       ai_addrlen := addrinfo.AddressLen;
        dirty_ptr ai_addrptr := addrinfo.Address;

        theListenSocket := new Socket(ai_family, ai_socktype, ai_protocol);
        
        theListenSocket.bind(ai_addrptr, ai_addrlen);
        
        reader.close();
        
        theListenSocket.listen(SOMAXCONN)
    }

    Enumerable<TcpRemoteClient> Clients = new Enumerable<TcpRemoteClient>
    { 
        /*embeddable dispatch => theClients.*/
        
        Enumerator<TcpRemoteClient> enumerator() => theClients; 
    };

    set Blocking(bool val)
    {
        int mode := 0;
        
        ifnot (val)
        {
            mode := 1
        };

        theListenSocket.iocommand(FIONBIO, ref mode)
    }    
    
    private TcpRemoteClient newClient()
    {
        auto socket := theListenSocket.accept();
        TcpRemoteClient client := nil;
        if (nil != socket)
        {
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
            
            client := TcpRemoteClient.assign(socket);
            
            theClients.append(client)
        };
        
        ^ client
    }
    
    private closeClient(TcpRemoteClient client, int index)
    {
        theClients.remove(index);
        
        client.close();
                
        if (nil != OnLeave)
            { OnLeave(client) }         
    }
    
    private handleError(SocketException e, TcpRemoteClient client, int n)
    {
        if (nil != OnError)
            { OnError(client, e) };
        
        int err := e.ErrorCode;
        if (err == WSAECONNRESET)
            { self.closeClient(client, n) }
    }
    
    private readClients()
    {
        int counter := theClients.Length;
        for (int i := 0, i < counter, i += 1) {
            TcpRemoteClient client := theClients[i];

            int rcvd := 0;
            try
            {
                client.read(theBuffer, theMaxLength, ref rcvd)
            }
            catch(SocketException e)
            {
                self.handleError(e, client, i)
            };
            
            if (rcvd == 0)
            { 
                self.closeClient(client, i) 
            }
            else
            { 
                if (rcvd > 0)
                {
                    if (nil != OnReceived)
                        { OnReceived(client,theBuffer,rcvd) }
                }
            }
        }
    }
    
    private bool proceed()
    {
        auto client := self.newClient();
        if (nil != client)
        {
            if(nil != OnConnect)
                { OnConnect(client) }
        };

        self.readClients();
        
        ^ true
    }
    
    start()
    {
        theListenThread := Thread.assign(
        {
            while (self.proceed()){}
        });
        
        theListenThread.start()
    }
    
    TcpServer sendToAll(byte[] buffer, int length)
    {
        int counter := theClients.Length;
        
        for(int i := 0, i < counter, i += 1)
        {
            TcpRemoteClient client := theClients[i];
            
            client.write(buffer, length)
        }
    }
    
    TcpServer sendToAll(string s)
    {
        byte buffer[256];
        int  len := s.Length;
        int  buf_len := 256;
        int  index := 0;
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
            
            self.sendToAll(buffer, buf_len);
            
            len := len - buf_len;
            index := index + buf_len
        }
    }
    
    TcpServer sendToAll(int n)
    {
        byte buffer[4];
        buffer.write(0, n);
        
        self.sendToAll(buffer, 4)
    }
    
    close()
    {
        theListenSocket.shutdown(SD_BOTH);
        
        theListenSocket.close()
    }
}