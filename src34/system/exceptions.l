// --- type --

type exception = Exception.

// --- helper ---

extension op
{
    throw
        => system'core_routines'__object.
}

// --- CallStack ---

/// A call stack
public sealed /*nonstructural */class CallStack
{
    intarray theStack.
    
    /// Creates a current call stack
    constructor new
    [
        T<int,64> aStack.
        int       aLength := 0.
        
        aLength := system'external core_callstack_load(T<intarray>(aStack), 64).
            
        theStack := IntArray new(0, aLength, T<intarray>(aStack))
    ]
    
    /// Generates the call stack literal representation
    T<literal> literal
    [
        int     anIndex := 0.
        int     aLength := theStack length.
        int     aBufferLength := 0.
        int     address := 0.

        T<byte,512> aBuffer.
        literal     aCallStackText := emptyLiteralValue.
        while (anIndex < aLength)
        [
            theStack read(anIndex, &address).
            
            aBufferLength := system'external LoadAddressInfo(address, T<bytearray>(aBuffer), 512).
            
            if (aBufferLength > 0)
                [ aCallStackText := aCallStackText add(LiteralValue fromByteArray(0, aBufferLength, T<bytearray>(aBuffer))); add(forwards'newLine) ].
            
            anIndex := anIndex + 1.
        ].
        
        ^ aCallStackText.
    ]
}

// --- Exception ---

/// A basic exception
public class Exception
{
    literal theMessage.
    object  theCallStack.
    
    /// Returns the message and the call stack
    T<literal> literal
    [
        literal retVal := theMessage add($10"Call stack:"$10).
        
        ^ retVal add(theCallStack literal)
    ]
    
    /// Returns the message
    T<literal> message = theMessage.
    
    /// Executes the exception handler for the current exception if available.
    ///
    /// Otherwise goes to the next exception handler
    catch : aCatchBlock
    [
        if(nil == aCatchBlock~__prober on:self)
            [ self throw ]
    ]    

    /// Executes the exception handler for the current exception if available.
    /// Finally code is called before
    ///
    /// Otherwise goes to the next exception handler
    catch(object aFinallyCode, object aCatchBlock)
    [
        aFinallyCode().
        
        if(nil == aCatchBlock~__prober on:self)
            [ self throw ]
    ]

    if : aCatchblock
    [
        ^ aCatchblock(self).
    ]

    raise
    [
        self throw.  
    ]
    
    raise : aFinallyCode
    [
        aFinallyCode().
        
        self throw.
    ]
    
//    subject = $self.
        
    /// Creates the exception with the provided message and error type
    constructor new(literal aMessage)
    [
        theMessage := aMessage.
        theCallStack := CallStack new.
    ]    
    
    constructor new
        <= new(self className).
}

// --- FormatException ---

public class FormatException :: Exception
{
    constructor new
        <= new("A number is not in the correct format").
        
    constructor new : message
        <= new(message).
}

// --- OutOfRangeException ---

public class OutOfRangeException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("An index is out of range").

    /// Creates the exception
    constructor new:aMessage
        <= new(aMessage).
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

public class InvalidArgumentException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("Invalid argument").
        
    /// Creates the exception
    constructor new:aMessage
        <= new(aMessage).
}

// --- InvalidOperationException ---

public class InvalidOperationException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("Invalid operation").

    /// Creates the exception
    constructor new:aMessage
        <= new(aMessage).
}

// --- MethodNotFound ---

public class MethodNotFoundException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("Method not found").

    stacksafe constructor new(mssg missingMessage)
        <= new("Method " add(missingMessage); add(" not found")).
        
    constructor new(object object, mssg missingMessage)
        <= new(object __className + " : Method " + missingMessage literal + " not found").
}

// --- MethodNotSupported ---

public class NotSupportedException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("Operation is not supported").
        
    /// Creates the exception
    constructor new :aMessage
        <= new(aMessage).
}

// --- AbortException ---

/// An abort exception 
public class AbortException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("The process terminated").
}

// --- CriticalException ---

public class CriticalException :: Exception
{
    /// Creates the exception
    constructor new
        <= new("Critical exception").
}