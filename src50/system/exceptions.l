////// --- type --
////
////type exception = Exception.
//
//// --- helper ---
//
//extension op
//{
//    throw()
//        => system'core_routines'__object;
//}

// --- CallStack ---

/// A call stack
public sealed class CallStack
{
    int[] theStack;
    
    /// Creates a current call stack
    constructor()
    {
        int stack[64];
        int length := 0;
        
        length := extern elenart.core_callstack_load(stack, 64);
          
        theStack := new int[](length);
        for(int i := 0, i < length, i += 1)
        {
            theStack[i] := stack[i]
        }
    }
    
//    /// Generates the call stack literal representation
//    get string Printable()
//    {
//        int    length := theStack.Length;
//        int    bufferLength := 0;
//        int    address := 0;
//
//        byte   buffer[512];
//        string callStackText := emptyString;
//        for (IntNumber i := 0, i < length, i += 1)
//        {
//            address := theStack[i];
//            
//            bufferLength := extern elenart.LoadAddressInfo(address, buffer, 512);
//            if (bufferLength > 0)
//            {
//                callStackText += String.fromByteArray(0, bufferLength, buffer).add(forward newLine)
//            }
//        };
//        
//        ^ callStackText
//    }
}

// --- Exception ---

/// A basic exception
public class Exception
{
    string    theMessage;
//    CallStack theCallstack;
//
//    /// Returns the message and the call stack
//    string toPrintable()
//    {
//        string retVal := theMessage.add($10"Call stack:"$10).add(theCallstack.Printable);
//        
//        ^ retVal
//    }
//    
//    /// Returns the message
//    string Message = theMessage;
    
    on(handler)
        => intern system'core_routines'__exception;

//    on(handler1,handler2)
//        => system'core_routines'__exception;
//
//    raise()
//    {
//        self.throw()
//    }
    
    /// Creates the exception with the provided message and error type
    constructor new(string message)
    {
        theMessage := message;
//        theCallstack := new CallStack()
    }    
    
//    constructor new()
//        <= new(self.__getClassName());
//
////    constructor raise()
////        <= new()
////    {
////        
////    }
}

// --- FormatException ---

public class FormatException : Exception
{
//    constructor new()
//        <= new("A number is not in the correct format");
//        
//    constructor new(string message)
//        <= new(message);
}

// --- OutOfRangeException ---

public class OutOfRangeException : Exception
{
//    static raise()
//    {
//        OutOfRangeException.new().raise()
//    }
//    
//    /// Creates the exception
//    constructor new()
//        <= new("An index is out of range");
//
//    /// Creates the exception
//    constructor new(string message)
//        <= new(message);
}

// --- InvalidArgumentException ---

public class InvalidArgumentException : Exception
{
    /// Creates the exception
    constructor new()
        <= new("Invalid argument");
        
    /// Creates the exception
    constructor new(string message)
        <= new(message);

    static raise()
    {
        InvalidArgumentException.new().raise()
    }
}

//// --- InvalidOperationException ---
//
//public class InvalidOperationException : Exception
//{
//    /// Creates the exception
//    constructor new()
//        <= new("Invalid operation");
//
//    /// Creates the exception
//    constructor new(string message)
//        <= new(message);
//
//    static raise()
//    {
//        InvalidOperationException.new().raise()
//    }
//}

// --- MethodNotFound ---

public sealed class MethodNotFoundException : Exception
{
    /// Creates the exception
//    constructor new()
//        <= new("Method not found");
//
//    constructor new(string message)
//        <= new(message);
//
//    constructor new(mssg missingMessage)
////        <= ("Method " + missingMessage.Printable + " not found");
//        <= new(cast string("Method " + missingMessage.Printable + " not found"));
        
    constructor new(object object, mssg missingMessage)
    {
    }
////        <= (object.__getClassName() + " : Method " + missingMessage.Printable + " not found");
//        <= new(object.__getClassName() + " : Method " + missingMessage.Printable + " not found");
}

//// --- NotSupportedException ---
//
//public class NotSupportedException : Exception
//{
//    static raise()
//    {
//        NotSupportedException.new().raise()
//    }
//    
//    /// Creates the exception
//    constructor new()
//        <= new("Operation is not supported");
//        
//    /// Creates the exception
//    constructor new(string message)
//        <= new(message);
//}

// --- AbortException ---

/// An abort exception 
public class AbortException : Exception
{
//    static raise()
//    {
//        AbortException.new().raise()
//    }
//    
//    /// Creates the exception
//    constructor new()
//        <= new("The process terminated");
}

//// --- CriticalException ---
//
//public class CriticalException : Exception
//{
//    /// Creates the exception
//    constructor new()
//        <= new("Critical exception");
//}
//
//// --- NilReferenceException ---
//
//public class NilReferenceException : CriticalException
//{
//    /// Creates the exception
//    constructor new()
//        <= new("Critical exception : an operation with nil");
//}