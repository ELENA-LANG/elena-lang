#import system.

// --- IOException ---

exceptionof(IOError) IOException
{
    constructor new &literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new &literal:"I/O error".
}
