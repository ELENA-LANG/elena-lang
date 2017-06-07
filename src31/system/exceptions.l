// --- helper ---

extension $op
{
    throw
        => system'core_routines'__object.
}

// --- CallStack ---

/// A call stack
sealed nonstructural class CallStack
{
    intarray theStack.
    
    /// Creates a current call stack
    constructor new
    [
        int aStack(64).
        int aLength := 0.
        
        aLength := system'external core_callstack_load
            intarray:aStack
            int:64.
            
        theStack := IntArray new int:0 int:aLength intarray:aStack
    ]
    
    /// Generates the call stack literal representation
    literal
    [
        int     anIndex := 0.
        int     aLength := theStack length.
        int     aBufferLength := 0.
        int     address := 0.

        byte    aBuffer(512).
        literal aCallStackText := emptyLiteralValue.
        while (anIndex < aLength)
        [
            theStack read int:anIndex vint:address.
            
            aBufferLength := system'external LoadAddressInfo
                int:address 
                bytearray:aBuffer
                int:512.
            
            if (aBufferLength > 0)
                [ aCallStackText := aCallStackText add literal:(LiteralValue $new int:0 int:aBufferLength bytearray:aBuffer); add literal:'newLine ].
            
            anIndex := anIndex + 1.
        ].
        
        ^ aCallStackText.
    ]
}

// --- exceptionof ---

template exceptionof :: subject
{
    literal theMessage.
    object  theCallStack.
    
    /// Returns the message and the call stack
    literal
    [
        literal retVal := theMessage add literal:$10"Call stack:"$10.
        
        ^ retVal add literal:(theCallStack literal)
    ]
    
    /// Returns the message
    message = theMessage.
    
    /// Executes the exception handler
    if subject:handler
    [
        handler eval:$self.
    ]

    /// Executes the generic exception handler
    if Error:aHandler
    [
        aHandler eval:$self.
    ]

    /// default handler
    generic if : aHandler
    [
        $self throw.
    ]
        
    /// Executes the exception handler for the current exception if available.
    ///
    /// Otherwise goes to the next exception handler
    on : aCatchBlock
    [
        aCatchBlock eval subject:$self | if MethodNotFoundError: (:e) [ $self throw. ].
    ]    

    /// Executes the exception handler for the current exception if available.
    /// Finally code is called before
    ///
    /// Otherwise goes to the next exception handler
    on : aFinallyCode : aCatchBlock
    [
        aFinallyCode eval.
        
        aCatchBlock eval subject:$self | if MethodNotFoundError: (:e) [ $self throw. ].
    ]    

    raise
    [
        $self throw.  
    ]
    
    raise : aFinallyCode
    [
        aFinallyCode eval.
        
        $self throw.
    ]
}

// --- Exception ---

/// A basic exception
class Exception
{
    literal theMessage.
    subject theError.
    object  theCallStack.

    /// Creates the exception with the provided message and error type
    constructor new literal:aMessage subject:anError
    [
        theMessage := aMessage.
        theError := anError.
        theCallStack := CallStack new.
    ]    
    
    /// Executes the generic exception handler
    if Error:aHandler
    [
        aHandler eval:$self.
    ]
    
    /// If incoming subject equals to the exception type, executes the exception handler.
    ///
    /// Otherwise goes to the next exception handler
    generic if : aHandler
    [
        ($subject == theError) ? [ aHandler eval:$self ] ! [ $self throw ]
    ]
    
    /// Executes the exception handler for the current exception if available.
    ///
    /// Otherwise goes to the next exception handler
    on : aCatchBlock
    [
        (aCatchBlock~theError eval:$self) back:$nil | if MethodNotFoundError: (:e) [ $self throw ]
    ]
    
    /// Returns the message and the call stack
    literal = theMessage add literal:$10"Call stack:"$10; add literal:theCallStack.
    
    /// Returns the message
    message = theMessage.

    raise
    [
        $self throw.  
    ]
}

// --- FormatException ---

class FormatException :: exceptionof(FormatError)
{
    constructor new
    [
        theMessage := "A number is not in the correct format".
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new:aMessage
    [
        theMessage := aMessage literal.
        theCallStack := CallStack new.
    ]
}

// --- OutOfRangeException ---

class OutOfRangeException :: exceptionof(OutOfRangeError)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"An index is out of range".

    /// Creates the exception
    constructor new:aMessage
        <= new literal:aMessage.
}

//// --- IndexOutOfRangeException ---
//
//#class(exceptionof:IndexOutOfRangeError) IndexOutOfRangeException
//{
//    #constructor new &literal:message
//    [
//        theMessage := message.
//        theCallStack := CallStack new.
//    ]
//    
//    /// Creates the exception
//    #constructor new
//        <= new &literal:"An index is out of range".
//
//    /// Creates the exception
//    #constructor new:aMessage
//        <= new &literal:(aMessage literal).
//}

// --- InvalidArgumentException ---

class InvalidArgumentException :: exceptionof(InvalidArgumentError)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"Invalid argument".
        
    /// Creates the exception
    constructor new:aMessage
        <= new literal:aMessage.
}

// --- InvalidOperationException ---

class InvalidOperationException :: exceptionof(InvalidOperationError)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"Invalid operation".

    /// Creates the exception
    constructor new:aMessage
        <= new literal:aMessage.
}

// --- MethodNotFound ---

class MethodNotFoundException :: exceptionof(MethodNotFoundError)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"Method not found".

    stacksafe constructor new mssg:missingMessage
        <= new literal:("Method " add literal:missingMessage; add literal:" not found").
        
    stacksafe constructor new object:object mssg:missingMessage
        <= new literal:(object className; add literal:" : Method "; add literal:missingMessage; add literal:" not found").
}

// --- MethodNotSupported ---

class NotSupportedException :: exceptionof(NotSupportedError)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"Operation is not supported".
        
    /// Creates the exception
    constructor new :aMessage
        <= new literal:aMessage.
}

// --- AbortException ---

/// An abort exception 
class AbortException :: exceptionof(Abort)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"The process terminated".
}
