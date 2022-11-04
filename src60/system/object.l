// === classes ===

// --- super object ---

public class Object
   : info<"a common ancestor">
{
   const string __className := $name __decl;

   constructor() 
      : info<"Creates the object">
   {
   }

   bool equal(o)
               : info<"Returns true if the specified object is equal to the current object; otherwise, false. By default compares the object references.">
      = self.equalReference(o);

   bool notequal(o)
               : info<"Returns true if the specified object is not equal to the current object; otherwise, false. By default it inverts the result of equal[2].">
      = !(self.equal(o));

   // virtual method declarations : forcing the output type                            
   predefined bool less(o);
   predefined bool greater(o);
   predefined bool notless(o);
   predefined bool notgreater(o);

   string toPrintable() 
               : info<"Returns the string representation. By default it returns the class name.">
      = __className;

   dispatch() : external<system'core_routines'__object_dispatcher>, info<"Implements a message dispatching in VMT">;
}

// --- basic extensions ---

public extension commonOp
{
   back(o)
            : info<"Returns the parameter">
      = o;

   bool equalReference(o) : external<system'core_routines'__object_equal>;

   then(Func f)
      = f();

   then(Func1 f)
      = f(self);
}

