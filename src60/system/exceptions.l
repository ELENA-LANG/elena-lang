// --- Exception ---
public class Exception
{
//   on((handler) : external<system'core_routines'__exception_dispatcher>;
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