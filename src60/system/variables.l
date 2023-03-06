// --- BaseVariable ---
public abstract class BaseVariable
   : info("variable base class")
{
   get abstract Value() : info("Returns the variable value;<br/>should be overridden");

   set abstract Value(o) : info("Set the variable value;<br/>should be overridden");
}

public sealed Reference<T> : BaseVariable,
   info("a variable template")
{
   embeddable T value;

   constructor(T val)
      : info("Initializes a variable")
   {
      value := val
   }

   get T Value()
      : info("Returns the variable value")
   {
      ^ value
   }

   set Value(T val)
      : info("Set the variable value")
   {
      value := val
   }

   T cast()
            : info("Returns the variable value")
      = value;

   bool equal(o)
            : info("Returns true if o is equal to the variable value; otherwise, false")
      => value;

   string toPrintable()
            : info("Returns the value string representation")
      => value;

   dispatch() => value;
}

public class BaseExtender
{
   object object;

   bool equal(o)
      => object;

   string toPrintable()
      => object;

   dispatch()
      => object;
}

public class Variable : BaseVariable, Reference<object>,
   info("A generic variable.<br/>Extends an assigned value")
{
   constructor()
         : info("Creates the empty variable")
   {
   }

   constructor(value)
         : info("Creates the variable")
   {
      this value := value
   }

   append(val)
   {
      if (value == nil) {
         value := val
      }
      else {
         value += val;
      }
   }

   reduce(val)
   {
      if (value == nil) {
         value := val
      }
      else {
         value -= val;
      }
   }

   multiplyBy(val)
   {
      if (value == nil) {
         system'InvalidOperationException.raise()
      };

      value *= val
   }

   divideBy(val)
   {
      if (value == nil) {
         system'InvalidOperationException.raise()
      };

      value /= val
   }
}
