// --- Symbol ---

public sealed const struct Symbol : info("A dynamic symbol reference")
{
   __ptr pointer;

   private load(string name) : external(system'core_routines'__symbol_load);
   private copy(pointer ptr) : external(system'core_routines'__symbol_copy);
   private bool compare(Symbol s) : external(system'core_routines'__symbol_equal);

   static Symbol Default : info("Returns null reference")
      = default;

   /// Loads a symbol
   constructor(string name) : info("Loads a symbol reference<br/>If the symbol cannot be loaded raises SymbolLoaderException")
   {
      excluded {
         self.load(name);
      }
   }   

   constructor(pointer ptr) : info("Copies the reference from a pointer<br/>Unsafe operation")
   {
      self.copy(ptr);
   }

   bool equal(Symbol s) : info("Returns true if the object and <b>s</b> refer both the same symbol<br/>Otherwise returns false")
      = self.compare(s);

   bool notequal(Symbol s) : info("Returns false if the object and <b>s</b> refer both the same symbol<br/>Otherwise returns true")
      = !self.compare(s);

   static Symbol tryLoad(string ns, string name) : info("Tries to load a symbol reference<br/>If the symbol cannot be loaded returns null-reference")
   {
      pointer ptr := default;

      excluded {
         ptr := extern LoadSymbolByString2LA(ns, name);
      };

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

public sealed const struct ClassReference : info("A dynamic class reference")
{
   __ptr pointer;

   private load(string name) : external(system'core_routines'__classref_load);
  
   constructor(string name)
      : info("Loads a class reference<br/>If the symbol cannot be loaded raises SymbolLoaderException")
   {
      excluded {
        self.load(name);
      }
   }
   
   dispatch() : external(system'core_routines'__classref_dispatcher);
}

// --- Extension ---

public mixin Extension : info("A mixin class extending a target with an extender")
{
    object target;
    object extender;
    
    constructor(target, extender) : info("Creates a mixin")
    {
        this target := target;
        this extender := extender;
    }
    
    dispatch() : external(system'core_routines'__extension);
}

// --- ExplicitExtension ---

public mixin ExtensionVariable : info("A mixin class extending a target with an extension")
{
   object target;
   object extension;
    
   constructor(target, extension) : info("Creates a mixin")
   {
      this target := target;
      this extension := extension;
   }
    
   dispatch() : external(system'core_routines'__extensionvar);
}
