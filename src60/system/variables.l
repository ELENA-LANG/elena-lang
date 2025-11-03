// --- BaseVariable ---
public abstract class BaseVariable
   : info("a variable base class")
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

public nonstructural Nullable<T>
{
   T _value;

   constructor()
   {
      _value := nil;
   }

   constructor(T value)
   {
      _value := value
   }

   T Value
      = _value;

   T cast()
      = _value;

   bool hasValue
      = _value != nil;

   bool equal(Nullable<T> val)
   {
      if (self.hasValue)
      {
         if (val.hasValue)
         {
            ^ _value == val.Value
         }
      }
      else {
         ^ !val.hasValue;
      };         

      ^ false
   }
}

public class BaseExtender
   : info("A base mixing.<br/>The class must be inherited to extend the target object with extra methods")
{
   object object;

   bool equal(o) : info("Redirects the message to <b>object</b> field")
      => object;

   string toPrintable() : info("Redirects the message to <b>object</b> field")
      => object;

   dispatch() : info("Redirects an income message to <b>object</b> field")
      => object;
}

public class Variable : BaseVariable, Reference<object>,
   info("A generic variable.<br/>Extends an assigned value with compound operators : append (+=), reduce (-=), multiplyBy (*=) and divideBy (/=)")
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
         : info("Implements compound operator += <br>If the value is nil, assigns the operand to it, otherwise sum them and assign the result to the variable")
   {
      if (value == nil) {
         value := val
      }
      else {
         value += val;
      }
   }

   reduce(val)
         : info("Implements compound operator -= <br>If the value is nil, assigns the operand to it, otherwise subtract <b>val</b> from the variable value and assign the result to the variable")
   {
      if (value == nil) {
         value := val
      }
      else {
         value -= val;
      }
   }

   multiplyBy(val)
         : info("Implements compound operator *= <br>If the value is nil, raises an exception, otherwise multiply the variable value by <b>val</b> and assign the result to the variable")
   {
      if (value == nil) {
         system'InvalidOperationException.raise()
      };

      value *= val
   }

   divideBy(val)
         : info("Implements compound operator /= <br>If the value is nil, raises an exception, otherwise divide the variable value by <b>val</b> and assign the result to the variable")
   {
      if (value == nil) {
         system'InvalidOperationException.raise()
      };

      value /= val
   }
}
