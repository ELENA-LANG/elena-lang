// --- Exception ---
public class Exception
   : info("A basic exception")
{
   string    message;
   CallStack callStack;

   constructor new(string message)
   {
      this message := message;
      this callStack := new CallStack(3);
   }

   constructor new()
      <= new(self.__getClassName());

   on(handler) : external(system'core_routines'__exception_handler); 

   fallback(arg)
      <= raise();  

   raise() : external(system'core_routines'__exception_raise);

    __throwop sealed static raise()
   {
       weak self.new().raise()
   }

   sealed static raise(string message)
   {
       weak self.new(message).raise()
   }

   string toPrintable()
      : info("Returns the message and the call stack")
   {
      string retVal := message.add($10"Call stack:"$10).add(callStack.toPrintable());
        
      ^ retVal
   }

   string Message = message;
}

// --- CriticalException ---
public class CriticalException : Exception
{
   constructor new()
      <= super new("Critical exception");
}

// --- CriticalException ---
public class NilReferenceException : Exception
{
   constructor new()
      <= super new("Nil reference exception");
}

// --- DivisionByZeroException ---
public class DivisionByZeroException : Exception
{
   constructor new()
      <= super new("Division by zero exception");
}

// --- OutOfMemoryException ---
public class OutOfMemoryException : Exception
{
}

// --- InvalidArgumentException ---
public class InvalidArgumentException : Exception
{
   constructor new() : info("Creates the exception")
      <= super new("Invalid argument");

   constructor new(string argument) : info("Creates the exception")
      <= super new($"an invalid argument {argument}");
}

// --- MethodNotFoundException ---
public class MethodNotFoundException : Exception
{
   constructor new(object target, message mssg)
      <= super new(target.__getClassName() + " : Method " + mssg.toPrintable() + " not found")
   {
   }
}

public class NotSupportedException : Exception
{
   /// Creates the exception
   constructor new()
      <= new("Operation is not supported");
        
   /// Creates the exception
   constructor new(string message)
      <= super new(message);
}

public class SymbolLoaderException : Exception
{
   /// Creates the exception
   constructor new(string symbolName)
      <= super new("Symbol " + symbolName + " cannot be loaded");
}

// --- InvalidOperationException ---

public class InvalidOperationException : Exception
{
   constructor new() : info("Creates the exception")
      <= new("Invalid operation");

   constructor new(string message) : info("Creates the exception")
      <= super new(message);
}

// --- OutOfRangeException ---
public class OutOfRangeException : Exception
{
   /// Creates the exception
   constructor new()
      <= new("An index is out of range");

   /// Creates the exception
   constructor new(string message)
      <= super new(message);
}

// --- StackOverflowException ---
public class StackOverflowException : Exception
{
   /// Creates the exception
   constructor new()
      <= new("Stack overflow");

   /// Creates the exception
   constructor new(string message)
   {
      this message := message;
   }

   string toPrintable()
      = message;
}

// --- AbortException ---

public class AbortException : Exception,
         info("An abort exception ")
{
   /// Creates the exception
   constructor new()
      <= super new("The process terminated");
}

// --- FormatException ---

public class FormatException : Exception
{
    constructor new()
        <= new("A number is not in the correct format");
        
    constructor new(string message)
        <= super new(message);
}

// --- TypeLoaderException ---

public class TypeLoaderException : Exception
{
   /// Creates the exception
   constructor new(string className)
      <= super new("Type " + className + " cannot be loaded");
}

// --- TypeLoaderException ---

public class MessageLoaderException : Exception
{
   /// Creates the exception
   constructor new(string messageName)
      <= super new("Message " + messageName + " cannot be loaded");
}

