// --- Exception ---
public class Exception
   : info("A basic exception")
{
   string    message;
   CallStack callStack;

   constructor new(string message) : info("Creates an exception with the provide <b>message</b>")
   {
      this message := message;
      this callStack := new CallStack(3);
   }

   constructor new() : info("Creates an exception with a class name as a message</b>")
      <= new(self.__getClassName());

   on(handler) : external(system'core_routines'__exception_handler); 

   fallback(arg)
      <= raise();  

   raise() : external(system'core_routines'__exception_raise);

    __throwop sealed static raise() : info("Raises an exception")
   {
       weak self.new().raise()
   }

   sealed static raise(string message) : info("Raises an exception with the provided <b>message</b>")
   {
       weak self.new(message).raise()
   }

   string toPrintable()
      : info("Returns the message and the call stack")
   {
      string retVal := message.add($10"Call stack:"$10).add(callStack.toPrintable());
        
      ^ retVal
   }

   string Message : info("Returns the exception message") = message;
}

// --- CriticalException ---
public class CriticalException : Exception, info("A critical exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= super new("Critical exception");
}

// --- CriticalException ---
public class NilReferenceException : Exception, info("A nil reference exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= super new("Nil reference exception");
}

// --- DivisionByZeroException ---
public class DivisionByZeroException : Exception, info("A division by zero reference exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= super new("Division by zero exception");
}

// --- OutOfMemoryException ---
public class OutOfMemoryException : Exception, info("Out of memory exception")
{
}

// --- InvalidArgumentException ---
public class InvalidArgumentException : Exception, info("Invalid argument exception")
{
   constructor new() : info("Creates the exception")
      <= super new("Invalid argument");

   constructor new(string argument) : info("Creates the exception")
      <= super new($"an invalid argument {argument}");
}

// --- MethodNotFoundException ---
public class MethodNotFoundException : Exception, info("Method not fount exception")
{
   constructor new(object target, message mssg) : info("Creates an exception with a default message")
      <= super new(target.__getClassName() + " : Method " + mssg.toPrintable() + " not found")
   {
   }
}

public class NotSupportedException : Exception, info("Not supported exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= new("Operation is not supported");
        
   constructor new(string message) : info("Creates an exception with the specified <b>message</b>")
      <= super new(message);
}

public class SymbolLoaderException : Exception, info("Symbol cannot be loaded exception")
{
   constructor new(string symbolName) : info("Creates an exception with the specified <b>symbolName</b>")
      <= super new("Symbol " + symbolName + " cannot be loaded");
}

// --- InvalidOperationException ---

public class InvalidOperationException : Exception, info("Invalid exception")
{
   constructor new() : info("Creates the exception")
      <= new("Invalid operation");

   constructor new(string message) : info("Creates the exception with the specified <b>message</b>")
      <= super new(message);
}

// --- OutOfRangeException ---
public class OutOfRangeException : Exception, info("Out of range exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= new("An index is out of range");

   constructor new(string message) : info("Creates the exception with the specified <b>message</b>")
      <= super new(message);
}

// --- StackOverflowException ---
public class StackOverflowException : Exception, info("Stack overflown exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= new("Stack overflow");

   constructor new(string message) : info("Creates the exception with the specified <b>message</b>")
   {
      this message := message;
   }

   string toPrintable()
      = message;
}

// --- AbortException ---

public class AbortException : Exception,
         info("An abort exception")
{
   constructor new() : info("Creates an exception with a default message")
      <= super new("The process terminated");
}

// --- FormatException ---

public class FormatException : Exception, info("Format exception")
{
    constructor new() : info("Creates an exception with a default message")
        <= new("A number is not in the correct format");
        
    constructor new(string message) : info("Creates the exception with the specified <b>message</b>")
        <= super new(message);
}

// --- TypeLoaderException ---

public class TypeLoaderException : Exception, info("Type cannot be loaded exception")
{
   constructor new(string className) : info("Creates the exception with the specified <b>className</b>")
      <= super new("Type " + className + " cannot be loaded");
}

// --- TypeLoaderException ---

public class MessageLoaderException : Exception, info("Message cannot be loaded exception")
{
   constructor new(string messageName) : info("Creates the exception with the specified <b>messageName</b>")
      <= super new("Message " + messageName + " cannot be loaded");
}

