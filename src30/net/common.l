#import system.

// --- types ---
#subject tcp_client       = net'TcpClient.
#subject tcp_remoteclient = net'TcpRemoteClient.
#subject tcp_server       = net'TcpServer.

exceptionof(TcpClientError) TcpClientException
{
    constructor new:message
    [
        theMessage := message literal.
        theCallStack := CallStack new.
    ]
}