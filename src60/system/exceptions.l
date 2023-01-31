// --- Exception ---
public class Exception
{
   string    message;
   CallStack callStack;

   constructor new(string message)
   {
      this message := message;
      this callStack := new CallStack();
   }

   constructor new()
      <= new(self.__getClassName());

   on(handler) : external(system'core_routines'__exception_handler); 

   fallback(arg)
      <= raise();  

   raise() : external(system'core_routines'__exception_raise);

   string toPrintable()
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
      <= new(message);
}

public class SymbolLoaderException
{
}

// --- InvalidOperationException ---

public class InvalidOperationException : Exception
{
   constructor new() : info("Creates the exception")
      <= new("Invalid operation");

   constructor new(string message) : info("Creates the exception")
      <= new(message);
}
