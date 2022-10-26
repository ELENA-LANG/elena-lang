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

   on(handler) : external<system'core_routines'__exception_handler>; 

   fallback(arg)
      <= raise();  

   raise() : external<system'core_routines'__exception_raise>;
}

// --- CriticalException ---
public class CriticalException : Exception
{
}

// --- CriticalException ---
public class NilReferenceException : Exception
{
}

// --- CriticalException ---
public class DivisionByZeroException : Exception
{
}

// --- CriticalException ---
public class OutOfMemoryException : Exception
{
}

// --- MethodNotFoundException ---
public class MethodNotFoundException : Exception
{
   constructor new(object target, message mssg)
//      <= new() 
   {
   }
}