// --- Symbol ---

public sealed const struct Symbol
{
   __ptr pointer;

   private load(string name) : external(system'core_routines'__symbol_load);
   private copy(pointer ptr) : external(system'core_routines'__symbol_copy);
   private bool compare(Symbol s) : external(system'core_routines'__symbol_equal);

   static Symbol Default
      = default;

   /// Loads a symbol
   constructor(string name)
   {
      self.load(name);
   }   

   constructor(pointer ptr)
   {
      self.copy(ptr);
   }

   bool equal(Symbol s)
      = self.compare(s);

   bool notequal(Symbol s)
      = !self.compare(s);

   static Symbol tryLoad(string ns, string name)
   {
      pointer ptr := extern LoadSymbolByString2LA(ns, name);
      if (ptr != 0)
      {
         ^ ptr
      }
      else
      {
         ^ default
      }
   }

   dispatch() : external(system'core_routines'__symbol_dispatcher);
}

// --- ClassReference ---

public sealed const struct ClassReference
{
   __ptr pointer;

   private load(string name) : external(system'core_routines'__classref_load);
  
   constructor(string name)
      : info("Loads a symbol")
   {
      self.load(name);
   }
   
   dispatch() : external(system'core_routines'__classref_dispatcher);
}

// --- Extension ---

public mixin Extension
{
    object target;
    object extender;
    
    constructor(target, extender)
    {
        this target := target;
        this extender := extender;
    }
    
    dispatch() : external(system'core_routines'__extension);
}

// --- ExplicitExtension ---

public mixin ExtensionVariable
{
   object target;
   object extension;
    
   constructor(target, extension)
   {
      this target := target;
      this extension := extension;
   }
    
   dispatch() : external(system'core_routines'__extensionvar);
}
