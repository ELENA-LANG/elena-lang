#import system.

// --- types ---
#subject tcp_client       = net'TcpClient.
#subject tcp_remoteclient = net'TcpRemoteClient.
#subject tcp_server       = net'TcpServer.

#class(exceptionof:TcpClientError) TcpClientException
{
    #constructor(stacksafe) new:message
    [
        theMessage := message literal.
        theCallStack := CallStack new.
    ]
}