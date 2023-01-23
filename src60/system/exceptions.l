// --- Exception ---
public class Exception
{
   string message;

   constructor new(string message)
   {
      this message := message
   }

   constructor new()
      <= new(self.__getClassName());

   on(handler) : external(system'core_routines'__exception_handler); 

   fallback(arg)
      <= raise();  

   raise() : external(system'core_routines'__exception_raise);

   string toPrintable()
   {
      string retVal := message/*.add($10"Call stack:"$10).add(theCallstack.toPrintable())*/;
        
      ^ retVal
   }
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
