// --- BaseVariable ---
public abstract class BaseVariable
   : info<"variable base class">
{
   get abstract Value() : info<"Returns the variable value;<br/>should be overridden">;

   set abstract Value(o) : info<"Set the variable value;<br/>should be overridden">;
}

public sealed Reference<T> : BaseVariable,
   info<"a variable template">
{
   embeddable T value;

   constructor(T val)
      : info<"Initializes a variable">
   {
      value := val
   }

   get T Value()
   {
      ^ value
   }

   set Value(T val)
   {
      value := val
   }
}

