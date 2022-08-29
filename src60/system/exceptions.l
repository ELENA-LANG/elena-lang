// --- Exception ---
public class Exception
{
   on(handler)
   {
      __probe handler(self, mssgconst fallback[2]);
   }

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
   {
   }
}