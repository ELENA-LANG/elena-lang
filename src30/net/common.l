// ==== ver 3.4.1 ===

#import system.

// --- types ---
#subject(class:net'TcpClient)tcp_client.
#subject(class:net'TcpServer)tcp_server.

#class TcpClientException :: Exception
{
    #constructor(stacksafe) new:aMessage
        <= new &literal:(aMessage literal) &subject: %TcpClientError.
}

