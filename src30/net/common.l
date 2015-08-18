#import system.

// --- types ---
#subject(class:net'TcpClient)tcp_client.

#class TcpClientException :: Exception
{
    #constructor(stacksafe) new:aMessage
        <= new &literal:(aMessage literal) &subject: %TcpClientError.
}

